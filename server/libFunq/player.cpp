/*
Copyright: SCLE SFE
Contributor: Julien Pagès <j.parkouss@gmail.com>

This software is a computer program whose purpose is to test graphical
applications written with the QT framework (http://qt.digia.com/).

This software is governed by the CeCILL v2.1 license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL v2.1 license and that you accept its terms.
*/

#include "player.h"
#include "player_utils.h"
#include "objectpath.h"

#include "dragndropresponse.h"
#include "shortcutresponse.h"

#include <QMetaMethod>
#include <QStringList>
#include <QWidget>
#include <QApplication>
#include <QBuffer>
#include <QDesktopWidget>
#include <QTimer>
#include <QTime>
#include <QTabBar>
#include <QKeyEvent>

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QWindow>
#endif

using namespace ObjectPath;

Player::Player(QIODevice *device, QObject *parent) :
    JsonClient(device, parent)
{
    connect(this, SIGNAL(emit_object_set_properties(QObject *, const QVariantMap &)),
            this, SLOT(_object_set_properties(QObject *, const QVariantMap &)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(emit_model_item_action(const QString &, QAbstractItemView *, const QModelIndex &)),
            this, SLOT(_model_item_action(const QString &, QAbstractItemView *, const QModelIndex &)),
            Qt::QueuedConnection);
}

qulonglong Player::registerObject(QObject *object) {
    if (!object) {
        return 0;
    }
    qulonglong id = (qulonglong) object;
    if (!m_registeredObjects.contains(id)) {
        connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(objectDeleted(QObject*)));
        m_registeredObjects[id] = object;
    }
    return id;
}

QObject * Player::registeredObject(const qulonglong &id) {
    return m_registeredObjects[id];
}

void Player::objectDeleted(QObject *object) {
    qulonglong id = (qulonglong) object;
    m_registeredObjects.remove(id);
}

QtJson::JsonObject Player::list_actions(const QtJson::JsonObject &) {
    const QMetaObject* metaObject = this->metaObject();
    QStringList methods;
    for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
        QMetaMethod method = metaObject->method(i);
        if (method.methodType() == QMetaMethod::Slot) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
            methods << QString::fromLatin1(metaObject->method(i).methodSignature());
#else
            methods << QString::fromLatin1(metaObject->method(i).signature());
#endif
        }
    }
    QtJson::JsonObject result;
    result["commands"] = methods;
    return result;
}

QtJson::JsonObject Player::widget_by_path(const QtJson::JsonObject & command) {
    QString path = command["path"].toString();
    QObject * o = findObject(path);
    qulonglong id = registerObject(o);
    if (id == 0) {
        return createError("InvalidWidgetPath", QString("Unable to find widget with path `%1`").arg(path));
    }
    QtJson::JsonObject result;
    result["oid"] = id;
    dump_object(o, result);
    return result;
}

QtJson::JsonObject Player::active_widget(const QtJson::JsonObject & command) {
    QObject * active;
    QString type = command["type"].toString();
    if (type == "modal") {
        active = QApplication::activeModalWidget();
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        if (! active) {
            active = QApplication::modalWindow();
        }
#endif
    } else if (type == "popup") {
        active = QApplication::activePopupWidget();
    } else if (type == "focus") {
        active = QApplication::focusWidget();
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        if (! active) {
            active = QApplication::focusWindow();
        }
#endif
    }else {
        active = QApplication::activeWindow();
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        if (! active) {
            QWindowList lst = QGuiApplication::topLevelWindows();
            if (! lst.isEmpty()) {
                active = lst.first();
            }
        }
#endif
    }
    if (! active) {
        return createError("NoActiveWindow",
                            QString::fromUtf8("There is no active widget (%1)").arg(type));
    }
    qulonglong id = registerObject(active);
    QtJson::JsonObject result;
    result["oid"] = id;
    dump_object(active, result);
    return result;
}

QtJson::JsonObject Player::object_properties(const QtJson::JsonObject & command) {
    ObjectLocatorContext ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QtJson::JsonObject result;
    dump_properties(ctx.obj, result);
    return result;
}

QtJson::JsonObject Player::object_set_properties(const QtJson::JsonObject & command) {
    ObjectLocatorContext ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QVariantMap properties = command["properties"].value<QVariantMap>();
    emit_object_set_properties(ctx.obj, properties);
    QtJson::JsonObject result;
    return result;
}

void Player::_object_set_properties(QObject * object, const QVariantMap & properties) {
    for(QtJson::JsonObject::const_iterator iter = properties.begin(); iter != properties.end(); ++iter) {
        object->setProperty(iter.key().toStdString().c_str(), iter.value());
    }
}

void recursive_list_widget(QWidget * widget, QtJson::JsonObject & out, bool with_properties) {
    QtJson::JsonObject resultWidgets, resultWidget;
    dump_object(widget, resultWidget, with_properties);
    foreach (QObject * obj, widget->children()) {
        QWidget * subWidget = qobject_cast<QWidget *>(obj);
        if (subWidget) {
            recursive_list_widget(subWidget, resultWidgets, with_properties);
        }
    }
    resultWidget["children"] = resultWidgets;
    out[objectName(widget)] = resultWidget;
}

QtJson::JsonObject Player::widgets_list(const QtJson::JsonObject & command) {
    bool with_properties = command["with_properties"].toBool();
    QtJson::JsonObject result;
    if (command.contains("oid")) {
        ObjectLocatorContext ctx(this, command, "oid");
        if (ctx.hasError()) { return ctx.lastError; }
        foreach (QObject * obj, ctx.obj->children()) {
            QWidget * subWidget = qobject_cast<QWidget *>(obj);
            if (subWidget) {
                recursive_list_widget(subWidget, result, with_properties);
            }
        }
    } else {
        QList<QWidget *> widgets = QApplication::topLevelWidgets();
        if (! widgets.isEmpty()) {
            foreach (QWidget * widget, widgets) {
                recursive_list_widget(widget, result, with_properties);
            }
        } else {
            // no qwidgets, this is probably a qtquick app - anyway, check for windows
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
            foreach (QWindow * window, QApplication::topLevelWindows()) {
                QtJson::JsonObject resultWindow;
                dump_object(window, resultWindow, with_properties);
                result[resultWindow["path"].toString()] = resultWindow;
            }
#endif
        }
    }
    return result;
}

QtJson::JsonObject Player::quit(const QtJson::JsonObject &) {
    if (qApp) {
        qApp->quit();
    }
    QtJson::JsonObject result;
    return result;
}

QtJson::JsonObject Player::widget_click(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QWidget> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QString action = command["mouseAction"].toString();
    QPoint pos = ctx.widget->rect().center();
    if (action == "doubleclick") {
        mouse_dclick(ctx.widget, pos);
    } else {
        mouse_click(ctx.widget, pos);
    }
    QtJson::JsonObject result;
    return result;
}

QtJson::JsonObject Player::widget_close(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QWidget> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }

    QTimer::singleShot(0, ctx.widget, SLOT(close()));

    QtJson::JsonObject result;
    return result;
}

QtJson::JsonObject Player::desktop_screenshot(const QtJson::JsonObject & command) {
    QString format = command["format"].toString();
    if (format.isEmpty()) {
        format = "PNG";
    }
    QPixmap window = QPixmap::grabWindow(QApplication::desktop()->winId());
    QBuffer buffer;
    window.save(&buffer, "PNG");

    QtJson::JsonObject result;
    result["format"] = format;
    result["data"] = buffer.data().toBase64();
    return result;
}

QtJson::JsonObject Player::widget_keyclick(const QtJson::JsonObject & command) {
    QWidget * widget;
    if (command.contains("oid")) {
        WidgetLocatorContext<QWidget> ctx(this, command, "oid");
        if (ctx.hasError()) { return ctx.lastError; }
        widget = ctx.widget;
    } else {
        widget = qApp->activeWindow();
    }
    QString text = command["text"].toString();
    for (int i=0; i<text.count(); ++i) {
        QChar ch = text[i];
        int key = (int) ch.toLatin1();
        qApp->postEvent(widget, new QKeyEvent(QKeyEvent::KeyPress, key, Qt::NoModifier, ch));
        qApp->postEvent(widget, new QKeyEvent(QKeyEvent::KeyRelease, key, Qt::NoModifier, ch));
    }
    QtJson::JsonObject result;
    return result;
}

DelayedResponse * Player::shortcut(const QtJson::JsonObject & command) {
    return new ShortcutResponse(this, command);
}

QtJson::JsonObject Player::tabbar_list(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QTabBar> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QStringList texts;
    for (int i=0; i< ctx.widget->count(); ++i) {
        texts << ctx.widget->tabText(i);
    }
    QtJson::JsonObject result;
    result["tabtexts"] = texts;
    return result;
}

DelayedResponse * Player::drag_n_drop(const QtJson::JsonObject & command) {
    return new DragNDropResponse(this, command);
}

QtJson::JsonObject Player::call_slot(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QWidget> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QString slot_name = command["slot_name"].toString();
    QVariant result_slot;
    bool invokedMeth = QMetaObject::invokeMethod(ctx.widget, slot_name.toLocal8Bit().data(),
                                      Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, result_slot),
                                      Q_ARG(QVariant, command["params"]));
    if (!invokedMeth) {
        return createError("NoMethodInvoked", QString::fromUtf8("The slot %1 could not be called")
                           .arg(slot_name));
    }

    QtJson::JsonObject result;
    result["result_slot"] = result_slot;
    return result;
}

QtJson::JsonObject Player::widget_activate_focus(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QWidget> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    activate_focus(ctx.widget);

    QtJson::JsonObject result;
    return result;
}
