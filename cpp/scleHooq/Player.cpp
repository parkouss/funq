/*
    Hooq: Qt4 UI recording, playback, and testing toolkit.
    Copyright (C) 2010  Mendeley Limited <copyright@mendeley.com>
    Copyright (C) 2009  Frederick Emmott <mail@fredemmott.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "Player.h"

#include "Event.h"

#include "ObjectHookName.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QCursor>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QShortcutEvent>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QWidget>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QMetaProperty>
#include "unistd.h"

#include <QTest>

#ifdef Q_OS_WIN32

#include <QDrag>
// Need to manually synthesise this
#include <QDropEvent>

#endif

namespace Hooq
{


QPointer<Player> Player::m_instance;

Player* Player::instance(QIODevice* device)
{
    m_instance = QPointer<Player>(new Player(device));
    return m_instance.data();
}

Player* Player::instance()
{
    return m_instance.data();
}

Player::Player(QIODevice* device)
    : QObject()
    , m_processingEvent(false)
    , m_pickWidget(NULL)
    , m_mode(Playback)
    , m_error(false)
{
    disconnect(device, 0, 0, 0);

    setDevice(device);
    connect(
                device,
                SIGNAL(readyRead()),
                SLOT(readNext())
                );

    // Start listening for events
    QInternal::registerCallback(QInternal::EventNotifyCallback, hook);
}

Player::~Player()
{
    // Remove our hook
    QInternal::unregisterCallback(QInternal::EventNotifyCallback, hook);
}

bool Player::hook(void** data)
{
    QObject* receiver = reinterpret_cast<QObject*>(data[0]);
    QEvent* event = reinterpret_cast<QEvent*>(data[1]);
    return instance()->eventFilter(receiver, event);
}

bool Player::eventFilter(QObject* receiver, QEvent* event)
{
    if(dynamic_cast<QDropEvent*>(event))
    {
        qDebug() << "IS A DROP EVENT";
        qDebug() << "Receiver:" << receiver;
        qDebug() << "Event type:" << event->type();
    }
    if(m_mode == Pick)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            return true;
        }
        if(event->type() == QEvent::MouseButtonRelease)
        {
            Q_ASSERT(device()->isWritable());
            device()->write("PICKED\n");
            QWidget* widget = qobject_cast<QWidget*>(receiver);
            if(widget && widget->focusProxy())
            {
                dumpProperties(widget->focusProxy());
            }
            else
            {
                qDebug() << "Dump des propriétés sur un object non supporté";
            }
            endPick();
            return true;
        }
    }
    return false;
}

void Player::readNext()
{
    if (hasError()) {
        m_error = true;
        m_return = errorString();
        ack();
        QXmlStreamReader::readNext();
        return;
    }

    while(!atEnd())
    {
        QXmlStreamReader::readNext();
        if(tokenType() == StartElement)
        {
            m_error = false;
            m_return.clear();

            if (m_processingEvent) {
                m_error = true;
                m_return = "Previous event not finished";

            } else {
                if (!handleElement()) {
                    m_error = true;
                    m_return = "Token non reconnu: " + name().toString();
                } else {
                    processEvents();
                }
            }
            ack();
        }
    }

    if (hasError()) {
        m_error = true;
        m_return = errorString();
        ack();
    }

    // Restart parser
    QIODevice * socket_handle = device();
    clear();
    setDevice(socket_handle);
}

QString _item_model_path(QAbstractItemModel * model, const QModelIndex & item) {
	QStringList path;
	QModelIndex parent = model->parent(item);
	while (parent.isValid()) {
		path << (QString::number(parent.row())
		         + "-"
		         + QString::number(parent.column()));
		parent = model->parent(parent);
	}
	// reverse list
	for(int k=0, s=path.size(), max=(s/2); k<max; k++) path.swap(k,s-(1+k));
	return path.join("/");
}

void _dump_item_model_attrs(QAbstractItemModel * model,
                      QXmlStreamWriter &xml,
                      const QModelIndex & index,
                      const QString & view_path) {
	xml.writeAttribute("view_path", view_path);
	QString path = _item_model_path(model, index);
    if (!path.isEmpty()) {
		xml.writeAttribute("path", path);
	}
    xml.writeAttribute("row", QString::number(index.row()));
    xml.writeAttribute("column", QString::number(index.column()));
    xml.writeAttribute("value", model->data( index ).toString());
}

void _dump_items_model(QAbstractItemModel * model,
                      QXmlStreamWriter &xml,
                      const QModelIndex & parent,
                      const QString & view_path) {

    for(int i = 0; i < model->rowCount(parent); i++) {
        for(int j = 0; j < model->columnCount(parent); j++) {
			QModelIndex index = model->index(i, j, parent);
			xml.writeStartElement("Item");
			_dump_item_model_attrs(model, xml, index, view_path);
            _dump_items_model(model, xml, index, view_path);
            xml.writeEndElement(); // Item
        }
    }
}

QModelIndex _get_model_item(QAbstractItemModel * model,
                            const QString & path,
                            int row,
                            int column) {
	QModelIndex parent;
	if (!path.isEmpty()) {
		QStringList parts = path.split("/");
		foreach (const QString & part, parts) {
			QStringList tmp = part.split("-");
			if (tmp.count() != 2) {
				return QModelIndex();
			}
			parent = model->index(tmp.at(0).toInt(),
			                      tmp.at(1).toInt(), parent);
			if (! parent.isValid()) {
				return parent;
			}
		}
	}
	
	return model->index(row, column, parent);
}

void Player::processEvents()
{
    if(m_processingEvent)
    {
        return;
    }

    m_processingEvent = true;
    while(!m_eventQueue.isEmpty())
    {
        Event * event = m_eventQueue.dequeue();
        if(!event->tag().isNull())
        {
            qDebug() << "Processing event" << event->tag();
        }

        ObjectPath* e = NULL;
        QObject* o = NULL;
        QWidget* w = NULL;
        QAbstractItemView * view = NULL;
        QAbstractItemModel * model = NULL;

        switch (event->targetType()) {
        case Event::NoTarget:
            break;

        case Event::Widget:
        case Event::Model:
            e = dynamic_cast<ObjectPath*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "ObjectPath cast failed";
            } else {
                o = findObject(e->path());
                if (o == NULL) {
                    m_error = true;
                    m_return = "Couldn't find receiver from path: " + e->path();
                } else if(event->targetType() == Event::Widget ||
                          event->targetType() == Event::Model) {
                    w = qobject_cast<QWidget*>(o);
                    if (w == NULL) {
                        m_error = true;
                        m_return = "Couldn't find widget from: " + e->path() + " - QObject: " + o->objectName();
                    } else if(event->targetType() == Event::Model) {
                        view = dynamic_cast<QAbstractItemView *>(w);
                        if (view == NULL) {
                            m_error = true;
                            m_return = "Couldn't find QAbstractItemView from: " + e->path() + " - QObject: " + o->objectName();
                        } else {
                            model = view->model();
                        }
                    }
                }
            }
        }

        if (m_error) {
            delete event;
            // Emptying eventQueue
            qDeleteAll(m_eventQueue);
            m_eventQueue.clear();
            continue;
        }

        // "break;" for ack, "continue;" for no-ack, "return;" to leave the event loop - be sure to re-enter it later
        switch(event->type())
        {
        case Event::NoOp:
            // no-op
            delete event;
            break;

        case Event::Sleep:
        {
            SleepEvent* e = dynamic_cast<SleepEvent*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "SleepEvent cast failed";
            } else {
                sleep(e->msec());
            }
            delete event;
            break;
        }
        case Event::DumpProperties:
        {
            dumpProperties(w);
            delete event;
            break;
        }
        case Event::SetProperty:
        {
            SetPropertyEvent* e = dynamic_cast<SetPropertyEvent*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "SleepEvent cast failed";
            } else {
                QVariant propValue = e->propValue();
                if (!propValue.isValid()) {
                    m_error = true;
                    m_return = "SetProperty: Type de property non geree";
                } else {
                
                    bool validProp = false;
                    const QMetaObject * meta = o->metaObject();
                    for (int i = 0; i<meta->propertyCount(); i++) {
                        if (e->propName() == meta->property(i).name()) {
                            validProp = true;
                            break;
                        }
                    }
                    
                    if (!validProp) {
                        m_error = true;
                        m_return = "SetProperty: property inexistante: " + e->propName();
                    } else {
                        o->setProperty(e->propName().toStdString().c_str(), propValue);
                    }
                }
            }
            delete event;
            break;
        }
        case Event::DumpWidgetsTree:
        {
            QXmlStreamWriter xml(device());
            xml.setAutoFormatting(true);
            xml.writeStartDocument();
            xml.writeStartElement("WidgetsTree");
            dumpWidgetsTree(xml);
            xml.writeEndElement(); // WidgetsTree
            xml.writeEndDocument();
            delete event;
            break;
        }
        case Event::DumpModel:
        {
            QXmlStreamWriter xml(device());
            xml.setAutoFormatting(true);
            xml.writeStartDocument();
            xml.writeStartElement("ModelItems");
            dumpWidget(xml, view);
            QString view_path = ObjectHookName::objectPath(view);
            _dump_items_model(model, xml, QModelIndex(), view_path);
            xml.writeEndElement(); // ModelItems
            xml.writeEndDocument();
            delete event;
            break;
        }

        case Event::GetWidget:
        {
            QXmlStreamWriter xml(device());
            xml.setAutoFormatting(true);
            xml.writeStartDocument();
            xml.writeStartElement("Widget");
            dumpWidget(xml, w);
            xml.writeEndElement(); // Widget
            xml.writeEndDocument();
            delete event;
            break;
        }

        case Event::Flush:
        {
            qDebug() << "Calling processEvents";
            QApplication::processEvents();
            delete event;
            continue;
        }
        case Event::Focus:
        {
            FocusEvent* e = dynamic_cast<FocusEvent*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "FocusEvent cast failed";
            } else {
                w->setFocus(e->reason());
            }
            delete event;
            break;
        }
        case Event::Close:
        {
            w->close();
            delete event;
            break;
        }
        case Event::Drop:
        {
            DropEvent* e = dynamic_cast<DropEvent*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "DropEvent cast failed";
            } else {
                // X11: Actual drag
                // Windows: avoid a press without release
                QMouseEvent* me = new QMouseEvent(
                            QEvent::MouseButtonRelease,
                            e->pos(),
                            w->mapToGlobal(e->pos()),
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier
                            );
                qApp->postEvent(o, me);
            }
            delete event;
            break;
        }
        case Event::Mouse:
        {
            MouseEvent* e = dynamic_cast<MouseEvent*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "MouseEvent cast failed";
            } else {
                QPoint pos = w->mapToGlobal(e->posOffset());

                switch(e->mouseEventType()) {
                case QEvent::MouseButtonPress:
                    qDebug() << "Posting button press to " << w << pos << e->button() << e->buttons();
                    break;
                case QEvent::MouseMove:
                    qDebug() << "Posting move event to " << w << pos << e->button() << e->buttons();
                    break;
                default:
                    break;
                }

                QMouseEvent* me = new QMouseEvent(
                            e->mouseEventType(),
                            e->posOffset(),
                            pos,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier
                            );

                qApp->postEvent(o, me);
            }
            delete event;
            break;
        }
        case Event::Pick:
            startPick();
            delete event;
            return;

        case Event::ModelRowCount:
            m_return = QString::number(model->rowCount());
            delete event;
            break;

        case Event::ModelColumnCount:
            m_return = QString::number(model->columnCount());
            delete event;
            break;

        case Event::ItemGet: {
			ModelItemEvent* e = dynamic_cast<ModelItemEvent*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "ModelItemEvent cast failed";
            } else {
				QModelIndex index = _get_model_item(model, e->itemPath(), e->row(), e->column());
				
				if (!index.isValid()) {
					m_error = true;
					m_return = QString("Unable to find the item at (%1, %2) with path \"%3\" in \"%4\"")
								.arg(e->row())
								.arg(e->column())
								.arg(e->itemPath())
								.arg(e->path());
				} else {
					QXmlStreamWriter xml(device());
					xml.setAutoFormatting(true);
					xml.writeStartDocument();
					QString view_path = ObjectHookName::objectPath(view);
					xml.writeStartElement("Item");
					_dump_item_model_attrs(model, xml, index, view_path);
					xml.writeEndElement(); // Item
					xml.writeEndDocument();
				}
			}
			
            delete event;
            break;
        }

        case Event::Shortcut:
        {
            ShortcutEvent * e = dynamic_cast<ShortcutEvent*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "ShortcutEvent cast failed";
            } else {
                o = findObject(e->path());
                if (o != NULL) {
                    w = qobject_cast<QWidget*>(o);
                } else {
                    w = qApp->activeWindow();
                }
                if (!w) {
                    m_error = true;
                    m_return = "An error occured while looking for a widget - no shortcut send";
                } else {
                    // taken from
                    // http://stackoverflow.com/questions/14283764/how-can-i-simulate-emission-of-a-standard-key-sequence
                    const QKeySequence & binding = e->key();
                    for (uint i = 0; i < binding.count(); ++i) {
                        uint key = binding[i];
                        QKeyEvent * key_e = new QKeyEvent(QKeyEvent::KeyPress, key & ~Qt::KeyboardModifierMask, key & Qt::KeyboardModifierMask);
                        qApp->postEvent(w, key_e);
                        key_e = new QKeyEvent(QKeyEvent::KeyRelease, key & ~Qt::KeyboardModifierMask, key & Qt::KeyboardModifierMask);
                        qApp->postEvent(w, key_e);
                    }
                }
            }
            
            delete event;
            break;
        }

        case Event::ItemSelect:
        case Event::ItemClick:
        case Event::ItemDClick:
        case Event::ItemEdit:
        {
            ModelItemEvent* e = dynamic_cast<ModelItemEvent*>(event);
            if (e == NULL) {
                m_error = true;
                m_return = "ModelItemEvent cast failed";
            } else {
				QModelIndex index = _get_model_item(model, e->itemPath(), e->row(), e->column());
				
				if (!index.isValid()) {
					m_error = true;
					m_return = QString("Unable to find the item at (%1, %2) with path \"%3\" in \"%4\"")
								.arg(e->row())
								.arg(e->column())
								.arg(e->itemPath())
								.arg(e->path());
				}
				else {
					view->scrollTo(index); // item visible
                    if (e->type() == Event::ItemSelect) {
                        view->setCurrentIndex(index);
                    }
                    else if (e->type() == Event::ItemEdit) {
                        view->setCurrentIndex(index);
                        view->edit(index);
                    }
                    else if (e->type() == Event::ItemClick) {
                        QRect visualRect = view->visualRect(index);
                        QTest::mouseClick(view->viewport(), Qt::LeftButton, 0, visualRect.center());
                    }
                    else if (e->type() == Event::ItemDClick) {
                        QRect visualRect = view->visualRect(index);
                        QTest::mouseDClick(view->viewport(), Qt::LeftButton, 0, visualRect.center());
                    }
                }
            }
            delete event;
            break;
        }
        }

    }
    m_processingEvent = false;

}

bool Player::handleElement()
{
    // "Magic" events
    if(name() == "noOp")
    {
        m_eventQueue.enqueue(new Event(Event::NoOp));
    }
    else if(name() == "dumpProperties")
    {
        m_eventQueue.enqueue(new GenericObjectEvent(Event::DumpProperties, Event::Widget, attributes().value("target").toString()));
    }
    else if (name() == "setProperty")
    {
        QString propType = attributes().value("propType").toString();
        QString propValue = attributes().value("propValue").toString();
        
        QVariant value;
        if (propType == "QString") {
            value = propValue;
        } else if (propType == "int") {
            value = propValue.toInt();
        } else if (propType == "double") {
            value = propValue.toDouble();
        } else if (propType == "bool") {
            value = (propValue == "true");
        }
        
        SetPropertyEvent * evt = new SetPropertyEvent(attributes().value("target").toString(),
                                                      attributes().value("propName").toString(),
                                                      value);
        m_eventQueue.enqueue(evt);
    }
    else if(name() == "dumpWidgetsTree")
    {
        m_eventQueue.enqueue(new Event(Event::DumpWidgetsTree));
    }
    else if(name() == "dumpModel")
    {
        m_eventQueue.enqueue(new GenericObjectEvent(Event::DumpModel, Event::Model, attributes().value("target").toString()));
    }
    else if(name() == "pick")
    {
        m_eventQueue.enqueue(new Event(Event::Pick));
    }
    else if(name() == "dragAndDrop")
    {
        postDragAndDrop();
    }
    else if(name() == "clickWidget")
    {
        postClick();
    }
    else if(name() == "getItem")
    {
        m_eventQueue.enqueue(new ModelItemEvent(
					Event::ItemGet,
					attributes().value("view_target").toString(),
					attributes().value("item_path").toString(),
					attributes().value("row").toString().toInt(),
					attributes().value("column").toString().toInt()
					));
    }
    else if(name() == "selectItem")
    {
        m_eventQueue.enqueue(new ModelItemEvent(
					Event::ItemSelect,
					attributes().value("view_target").toString(),
					attributes().value("item_path").toString(),
					attributes().value("row").toString().toInt(),
					attributes().value("column").toString().toInt()
					));
    }
    else if(name() == "clickItem")
    {
        m_eventQueue.enqueue(new ModelItemEvent(
					Event::ItemClick,
					attributes().value("view_target").toString(),
					attributes().value("item_path").toString(),
					attributes().value("row").toString().toInt(),
					attributes().value("column").toString().toInt()
					));
    }
    else if(name() == "dClickItem")
    {
        m_eventQueue.enqueue(new ModelItemEvent(
					Event::ItemDClick,
					attributes().value("view_target").toString(),
					attributes().value("item_path").toString(),
					attributes().value("row").toString().toInt(),
					attributes().value("column").toString().toInt()
					));
    }
    else if(name() == "editItem")
    {
        m_eventQueue.enqueue(new ModelItemEvent(
					Event::ItemEdit,
					attributes().value("view_target").toString(),
					attributes().value("item_path").toString(),
					attributes().value("row").toString().toInt(),
					attributes().value("column").toString().toInt()
					));
    }
    else if(name() == "getWidget")
    {
        m_eventQueue.enqueue(new GenericObjectEvent(Event::GetWidget, Event::Widget, attributes().value("target").toString()));
    }
    else if(name() == "rowCount")
    {
        m_eventQueue.enqueue(new GenericObjectEvent(Event::ModelRowCount, Event::Model, attributes().value("target").toString()));
    }
    else if(name() == "columnCount")
    {
        m_eventQueue.enqueue(new GenericObjectEvent(Event::ModelColumnCount, Event::Model, attributes().value("target").toString()));
    }


    // QEvents
    else if(name() == "windowClosed")
    {
        m_eventQueue.enqueue(new GenericObjectEvent(Event::Close, Event::Widget, attributes().value("target").toString()));
    }
    else if(name() == "contextMenu")
    {
        m_eventQueue.enqueue(new ContextMenuEvent(attributes().value("target").toString(),
                                                  static_cast<QContextMenuEvent::Reason>(attributes().value("reason").toString().toInt()),
                                                  QPoint(
                                                      attributes().value("x").toString().toInt(),
                                                      attributes().value("y").toString().toInt()
                                                      ),
                                                  static_cast<Qt::KeyboardModifiers>(attributes().value("modifiers").toString().toInt())));
    }
    else if(name() == "focusChanged")
    {
        const Qt::FocusReason reason = static_cast<Qt::FocusReason>(attributes().value("reason").toString().toInt());
        const QString target = attributes().value("target").toString();
        m_eventQueue.enqueue(new FocusEvent(target, reason));
    }
    else if(name() == "keyPress")
    {
        postKeyEvent(QEvent::KeyPress);
    }
    else if(name() == "keyRelease")
    {
        postKeyEvent(QEvent::KeyRelease);
    }
    else if(name() == "mouseMove")
    {
        postMouseEvent(QEvent::MouseMove);
    }
    else if(name() == "mouseButtonPress")
    {
        postMouseEvent(QEvent::MouseButtonPress);
    }
    else if(name() == "mouseButtonDoubleClick")
    {
        postMouseEvent(QEvent::MouseButtonDblClick);
    }
    else if(name() == "mouseButtonRelease")
    {
        postMouseEvent(QEvent::MouseButtonRelease);
    }
    else if(name() == "mouseWheel")
    {
        m_eventQueue.enqueue(new MouseWheelEvent(attributes().value("target").toString(),
                                                 attributes().value("delta").toString().toInt(),
                                                 static_cast<Qt::MouseButtons>(attributes().value("buttons").toString().toInt()),
                                                 static_cast<Qt::KeyboardModifiers>(attributes().value("modifiers").toString().toInt()),
                                                 attributes().value("orientation") == "vertical" ? Qt::Vertical : Qt::Horizontal));
    }
    else if(name() == "shortcut")
    {
        m_eventQueue.enqueue(new ShortcutEvent(attributes().value("target").toString(),
                                               QKeySequence::fromString(attributes().value("keySequence").toString())));
    } else {
        return false;
    }

    return true;
}


void Player::postKeyEvent(int type)
{
    m_eventQueue.enqueue(new KeyEvent(attributes().value("target").toString(),
                                      static_cast<QEvent::Type>(type),
                                      attributes().value("key").toString().toInt(),
                                      static_cast<Qt::KeyboardModifiers>(attributes().value("modifiers").toString().toInt()),
                                      attributes().value("text").toString(),
                                      attributes().value("isAutoRepeat") == "true",
                                      attributes().value("count").toString().toUShort()));
}

void Player::postMouseEvent(QEvent::Type type)
{
    m_eventQueue.enqueue(new MouseEvent(attributes().value("target").toString(),
                                        type,
                                        static_cast<Qt::MouseButton>(attributes().value("button").toString().toInt()),
                                        static_cast<Qt::MouseButtons>(attributes().value("buttons").toString().toInt()),
                                        static_cast<Qt::KeyboardModifiers>(attributes().value("modifiers").toString().toInt())));
}

void Player::startPick()
{
    m_mode = Pick;
    m_pickWidget = new QLabel(tr("Click on a widget to retrieve its properties."));
    m_pickWidget->show();
    m_pickWidget->raise();
    // Crosshair
    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
}

void Player::endPick()
{
    m_mode = Playback;
    delete m_pickWidget;
    m_pickWidget = NULL;

    // Remove our crosshair
    QApplication::restoreOverrideCursor();
    // Continue with our queue
    m_processingEvent = false;
}

void Player::dumpWidget(QXmlStreamWriter & streamWriter, QWidget* widget) {
    streamWriter.writeAttribute("name", ObjectHookName::objectName(widget));
    streamWriter.writeAttribute("class_type", widget->metaObject()->className());
    
    const QMetaObject * mo = widget->metaObject();
    QStringList qt_class_type;
    while (mo) {
        const char * class_name = mo->className();
        if (class_name && class_name[0] == 'Q') {
            // on suppose qu'une classe commencant par Q est une classe de Qt
            qt_class_type << class_name;
        }
        mo = mo->superClass();
    }
    streamWriter.writeAttribute("qt_class_types", qt_class_type.join(","));
    streamWriter.writeAttribute("path", ObjectHookName::objectPath(widget));
}


void Player::dumpWidgetsTree(QXmlStreamWriter & streamWriter, QWidget* root)
{
    QList<QWidget*> children;
    if(root == NULL)
    {
        children = QApplication::topLevelWidgets();
    }
    else
    {
        Q_FOREACH(QObject* object, root->children())
        {
            QWidget* widget = qobject_cast<QWidget*>(object);
            if(widget)
            {
                children.append(widget);
            }
        }
    }

    Q_FOREACH(QWidget* widget, children)
    {
        streamWriter.writeStartElement("Widget");

        dumpWidget(streamWriter, widget);

        dumpWidgetsTree(streamWriter, widget);

        streamWriter.writeEndElement(); // Widget;
    }
}

void Player::dumpProperties(QWidget* widget)
{
    QXmlStreamWriter xml(device());
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("Properties");
    dumpWidget(xml, widget);
    for(int i = 0; i < widget->metaObject()->propertyCount(); ++i)
    {
        QMetaProperty property = widget->metaObject()->property(i);

        xml.writeStartElement("Property");
        xml.writeAttribute("name", property.name());
        xml.writeAttribute("type", property.typeName());

        switch(property.type())
        {
        case QVariant::Bool:
            xml.writeAttribute("value", property.read(widget).toBool() ? "true" : "false");
            break;
        default:
            xml.writeAttribute("value", property.read(widget).toString());
            break;
        };

        xml.writeEndElement(); // Property
    }

    xml.writeEndElement(); // Properties;
    xml.writeEndDocument();
}

void Player::postDragAndDrop()
{
    // Make sure that any events you queue up have setAck(false) called, except for the last, otherwise the GUI gets sent multiple ACKs for one command
    const QString sourcePath = attributes().value("source").toString();
    const QPoint sourcePoint = QPoint(attributes().value("sourceX").toString().toInt(), attributes().value("sourceY").toString().toInt());
    const QString targetPath = attributes().value("target").toString();
    const QPoint targetPoint = QPoint(attributes().value("targetX").toString().toInt(), attributes().value("targetY").toString().toInt());
    readElementText();
    qDebug() << "Read drag and drop event" << sourcePath << sourcePoint << targetPath << targetPoint;

    // These explicit flushes are an alternative to zero-second sleeps
#ifndef Q_OS_WIN32
    m_eventQueue.enqueue(new Event(Event::Flush));
#endif
    // 1. Start the drag
    // 1.1 Give focus
    m_eventQueue.enqueue(
                Event::addTag("dnd_sourceFocus", new FocusEvent(
                                  sourcePath,
                                  Qt::OtherFocusReason
                                  )
                              )
                );

    // 1.1bis Move mouse to starting point
    m_eventQueue.enqueue(
                Event::addTag("dnd_nudge", new MouseEvent(
                                  sourcePath,
                                  QEvent::MouseMove,
                                  Qt::NoButton,
                                  Qt::LeftButton,
                                  Qt::NoModifier,
                                  sourcePoint)
                              )
                );

    // 1.2. Click
#ifndef Q_OS_WIN32
    m_eventQueue.enqueue(new Event(Event::Flush));
#endif
    m_eventQueue.enqueue(
                Event::addTag("dnd_click", new MouseEvent(
                                  sourcePath,
                                  QEvent::MouseButtonPress,
                                  Qt::LeftButton,
                                  Qt::LeftButton,
                                  Qt::NoModifier
                                  )
                              )
                );
    // 1.3. Give it a little nudge (QAbstractItemView requires two passes through mouseMoveEvent to start the drag)
    m_eventQueue.enqueue(
                Event::addTag("dnd_nudge", new MouseEvent(
                                  sourcePath,
                                  QEvent::MouseMove,
                                  Qt::NoButton,
                                  Qt::LeftButton,
                                  Qt::NoModifier,
                                  QPoint(0, 1))
                              )
                );
    // 1.3. (reprise) Wait for drag time interval
    m_eventQueue.enqueue(Event::addTag("dnd_waitBeforeDrag", new SleepEvent(qApp->startDragTime() + 1))); // +1 here is just in case someone else has a similar issue to the QAbstractItemView one below
    // 1.4. Drag it for the minimum distance
    m_eventQueue.enqueue(
                Event::addTag("dnd_initialDrag", new MouseEvent(
                                  sourcePath,
                                  QEvent::MouseMove,
                                  Qt::NoButton,
                                  Qt::LeftButton,
                                  Qt::NoModifier,
                                  QPoint(0, qApp->startDragDistance() + 1)) // QAbstractItemView needs the +1, at least in 4.6.0
                              )
                );
    // 2. Now we've started the drag (hopefully), drop it
    // DO NOT INSERT ANY MORE FLUSHES; once the drag starts, at least on X11, we don't get another
    // chance to put stuff onto the event queue.
    ///@todo Check that a drag has actually started
    // 2.1. Move the mouse first, just to be friendly
    m_eventQueue.enqueue(
                Event::addTag("dnd_mainDrag", new MouseEvent(
                                  targetPath,
                                  QEvent::MouseMove,
                                  Qt::NoButton,
                                  Qt::LeftButton,
                                  Qt::NoModifier,
                                  targetPoint)
                              )
                );
    // 2.2. Now, release the hounds^Wmouse button
    m_eventQueue.enqueue(
                Event::addTag("dnd_drop",new DropEvent(
                                  targetPath,
                                  targetPoint)
                              )
                );

    // ACK the DnD as a whole
    m_eventQueue.enqueue(new Event(Event::Flush));
}

void Player::postClick() {
    const QString target = attributes().value("target").toString();
    QPoint point = QPoint(attributes().value("x").toString().toInt(),
                          attributes().value("y").toString().toInt());

    if (point.isNull()) {
        point = QPoint(10,10);
    }

    // Decompose le click:
    // 1 - Déplacement
    m_eventQueue.enqueue(new MouseEvent(target,
                                        QEvent::MouseMove,
                                        Qt::NoButton,
                                        Qt::LeftButton,
                                        Qt::NoModifier,
                                        point));

    // 2 - Press
    m_eventQueue.enqueue(new MouseEvent(target,
                                        QEvent::MouseButtonPress,
                                        Qt::LeftButton,
                                        Qt::LeftButton,
                                        Qt::NoModifier,
                                        point));

    // 3 - Release
    m_eventQueue.enqueue(new MouseEvent(target,
                                        QEvent::MouseButtonRelease,
                                        Qt::NoButton,
                                        Qt::LeftButton,
                                        Qt::NoModifier,
                                        point));

}

QObject* Player::findObject(const QString& path)
{
    const QString separator("::");
    QStringList parts = path.split(separator);
    if(parts.isEmpty())
    {
        return 0;
    }
    const QString name = parts.takeLast();
    QObject* parent = 0;
    if(parts.isEmpty())
    {
        // Top level widget
        Q_FOREACH(QWidget* widget, QApplication::topLevelWidgets())
        {
            if(ObjectHookName::objectName(widget) == name)
            {
                return widget;
            }
        }
        return 0;
    }
    else
    {
        parent = findObject(parts.join(separator));
        if(!parent)
        {
            return 0;
        }
    }

    Q_FOREACH(QObject* child, parent->children())
    {
        if(ObjectHookName::objectName(child) == name)
        {
            return child;
        }
    }

    return 0;
}

void Player::ack()
{
    if (!m_return.isEmpty()) {
        device()->write(m_return.toAscii() + "\n");
    }

    if (m_error) {
        device()->write("ERROR\n");
    } else {
        device()->write("ACK\n");
    }
}
} // namespace
