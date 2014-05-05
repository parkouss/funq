#include "player.h"
#include <QDebug>
#include <QMetaMethod>
#include <QStringList>
#include <QWidget>
#include "objectpath.h"
#include <QApplication>
#include <QMouseEvent>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QBuffer>
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTimer>

void mouse_click(QWidget * w, const QPoint & pos) {
    QPoint global_pos = w->mapToGlobal(pos);
    qApp->postEvent(w,
        new QMouseEvent(QEvent::MouseButtonPress,
                        pos,
                        global_pos,
                        Qt::LeftButton,
                        Qt::NoButton,
                        Qt::NoModifier));
    qApp->postEvent(w,
        new QMouseEvent(QEvent::MouseButtonRelease,
                        pos,
                        global_pos,
                        Qt::LeftButton,
                        Qt::NoButton,
                        Qt::NoModifier));
}

void mouse_dclick(QWidget * w, const QPoint & pos) {
    mouse_click(w, pos);
    qApp->postEvent(w,
        new QMouseEvent(QEvent::MouseButtonDblClick,
                        pos,
                        w->mapToGlobal(pos),
                        Qt::LeftButton,
                        Qt::NoButton,
                        Qt::NoModifier));
}

void dump_properties(QObject * object, QtJson::JsonObject & out) {
    const QMetaObject * metaobject = object->metaObject();
    for (int i = 0; i < metaobject->propertyCount(); ++i) {
        QMetaProperty prop = metaobject->property(i);
        QVariant value = object->property(prop.name());
        // first try to serialize and only add property if it is possible
        bool success = false;
        QtJson::serialize(value, success);
        if (success) {
            out[prop.name()] = value;
        }
    }
}

void dump_object(QObject * object, QtJson::JsonObject & out, bool with_properties = false) {
    out["path"] = objectPath(object);
    QStringList classes;
    const QMetaObject * mo = object->metaObject();
    while (mo) {
        classes << mo->className();
        mo = mo->superClass();
    }
    out["classes"] = classes;
    if (with_properties) {
        QtJson::JsonObject properties;
        dump_properties(object, properties);
        out["properties"] = properties;
    }
}

QString item_model_path(QAbstractItemModel * model, const QModelIndex & item) {
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


void dump_item_model_attrs(QAbstractItemModel * model,
                      QtJson::JsonObject & out,
                      const QModelIndex & index,
                      const qulonglong & viewid) {
    out["viewid"] = viewid;
    QString path = item_model_path(model, index);
    if (!path.isEmpty()) {
        out["itempath"] = path;
    }
    out["row"] = index.row();
    out["column"] = index.column();
    out["value"] = model->data( index ).toString();

    QVariant checkable = model->data(index, Qt::CheckStateRole);
    if (checkable.isValid()) {
        Qt::CheckState state = static_cast<Qt::CheckState>(
                    checkable.toUInt());
        QString stringState;
        switch(state) {
            case Qt::Unchecked: stringState = "unchecked"; break;
            case Qt::PartiallyChecked: stringState = "partiallyChecked"; break;
            case Qt::Checked: stringState = "checked"; break;
        }
        out["check_state"] = stringState;
    }
}

void dump_items_model(QAbstractItemModel * model,
                      QtJson::JsonObject & out,
                      const QModelIndex & parent,
                      const qulonglong & viewid,
                      bool recursive = true) {

    QtJson::JsonArray items;
    for(int i = 0; i < model->rowCount(parent); ++i) {
        for(int j = 0; j < model->columnCount(parent); ++j) {
            QModelIndex index = model->index(i, j, parent);
            QtJson::JsonObject item;
            dump_item_model_attrs(model, item, index, viewid);
            if (recursive && model->hasChildren(index)) {
                dump_items_model(model, item, index, viewid);
            }
            items << item;
        }
    }
    out["items"] = items;
}

QModelIndex get_model_item(QAbstractItemModel * model,
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

void dump_graphics_items(const QList<QGraphicsItem *>  & items, const qulonglong & viewid, QtJson::JsonObject & out) {
    QtJson::JsonArray outitems;
    foreach (QGraphicsItem * item, items) {
        QtJson::JsonObject outitem;
        outitem["stackpath"] = graphicsItemPath(item);
        outitem["viewid"] = viewid;
        QObject * itemObject = dynamic_cast<QObject *>(item);
        if (itemObject) {
            const QMetaObject * mo = itemObject->metaObject();
            QStringList classes;
            while (mo) {
                classes << mo->className();
                mo = mo->superClass();
            }
            outitem["classes"] = classes;
            outitem["objectname"] = itemObject->objectName();
        }
        dump_graphics_items(item->children(), viewid, outitem);
        outitems << outitem;
    }
    out["items"] = outitems;
}

QPoint pointFromString(const QString & data) {
    QStringList splitted = data.split(",");
    if (splitted.count() == 2) {
        QPoint p;
        p.setX(splitted[0].toInt());
        p.setY(splitted[1].toInt());
    }
    return QPoint();
}

Player::Player(QIODevice *device, QObject *parent) :
    JsonClient(device, parent)
{
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
            methods << QString::fromLatin1(metaObject->method(i).signature());
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
        return createError("InvalidWidgetPath", QString("Le widget est introuvable pour le path `%1`").arg(path));
    }
    QtJson::JsonObject result;
    result["oid"] = id;
    dump_object(o, result);
    return result;
}

#define FIND_OBJECT_OR_RETURN(command, oidKey, obj, id) \
    qulonglong id = command[oidKey].value<qulonglong>(); \
    QObject * obj = registeredObject(id); \
    if (!obj) { \
        return createError("NotRegisteredObject", QString::fromUtf8("L'objet (id:%1) n'est pas enregistré ou a été détruit").arg(id)); \
    }

#define FIND_WIDGET_OR_RETURN(command, oidKey, WIDGET_CLASS, widget, obj, id) \
    FIND_OBJECT_OR_RETURN(command, oidKey, obj, id) \
    WIDGET_CLASS * widget = qobject_cast<WIDGET_CLASS *>(obj); \
    if (!widget) { \
        return createError("NotAWidget", QString::fromUtf8("L'objet (id:%1) n'est pas un ## WIDGET_CLASS ##").arg(id)); \
    }

QtJson::JsonObject Player::object_properties(const QtJson::JsonObject & command) {
    FIND_OBJECT_OR_RETURN(command, "oid", obj, id)
    QtJson::JsonObject result;
    dump_properties(obj, result);
    return result;
}

QtJson::JsonObject Player::object_set_properties(const QtJson::JsonObject & command) {
    FIND_OBJECT_OR_RETURN(command, "oid", obj, id)
    QtJson::JsonObject properties = command["properties"].value<QtJson::JsonObject>();
    for(QtJson::JsonObject::const_iterator iter = properties.begin(); iter != properties.end(); ++iter) {
        obj->setProperty(iter.key().toStdString().c_str(), iter.value());
    }
    QtJson::JsonObject result;
    return result;
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
        FIND_WIDGET_OR_RETURN(command, "oid", QWidget, root, obj, id);
        foreach (QObject * obj, root->children()) {
            QWidget * subWidget = qobject_cast<QWidget *>(obj);
            if (subWidget) {
                recursive_list_widget(subWidget, result, with_properties);
            }
        }
    } else {
        foreach (QWidget * widget, QApplication::topLevelWidgets()) {
            recursive_list_widget(widget, result, with_properties);
        }
    }
    return result;
}

QtJson::JsonObject Player::quit(const QtJson::JsonObject & command) {
    if (qApp) {
        qApp->quit();
    }
    return command;
}

QtJson::JsonObject Player::widget_click(const QtJson::JsonObject & command) {
    FIND_WIDGET_OR_RETURN(command, "oid", QWidget, widget, obj, id);
    QString action = command["action"].toString();
    QPoint pos = widget->rect().center();
    if (action == "doubleclick") {
        mouse_dclick(widget, pos);
    } else {
        mouse_click(widget, pos);
    }
    QtJson::JsonObject result;
    return result;
}

QtJson::JsonObject Player::model_items(const QtJson::JsonObject & command) {
    QtJson::JsonObject result;
    FIND_WIDGET_OR_RETURN(command, "oid", QAbstractItemView, view, obj, id);
    QAbstractItemModel * model = view->model();
    if (!model) {
        return createError("MissingModel", QString::fromUtf8("La vue (id:%1) ne possède pas de modèle.").arg(id));
    }
    bool recursive = ! (model->inherits("QTableModel") || view->inherits("QTableView") || view->inherits("QListView"));
    dump_items_model(model, result, QModelIndex(), id, recursive);
    return result;
}

QtJson::JsonObject Player::model_item_action(const QtJson::JsonObject & command) {
    FIND_WIDGET_OR_RETURN(command, "oid", QAbstractItemView, view, obj, id);
    QAbstractItemModel * model = view->model();
    if (!model) {
        return createError("MissingModel", QString::fromUtf8("La vue (id:%1) ne possède pas de modèle.").arg(id));
    }
    QModelIndex index = get_model_item(model, command["itempath"].toString(), command["row"].toInt(), command["column"].toInt());
    if (!index.isValid()) {
        return createError("MissingModelItem", QString::fromUtf8("Impossible de trouver l'item identifié par %1").arg(command["itempath"].toString()));
    }
    view->scrollTo(index); // item visible
    QString itemaction = command["itemaction"].toString();
    if (itemaction == "select") {
        view->setCurrentIndex(index);
    } else if (itemaction == "edit") {
        view->setCurrentIndex(index);
        view->edit(index);
    } else if (itemaction == "click") {
        QRect visualRect = view->visualRect(index);
        mouse_click(view->viewport(), visualRect.center());
    } else if (itemaction == "doubleclick") {
        QRect visualRect = view->visualRect(index);
        mouse_dclick(view->viewport(), visualRect.center());
    } else {
        return createError("MissingItemAction", QString::fromUtf8("itemaction %1 inconnue").arg(itemaction));
    }
    QtJson::JsonObject result;
    return result;
}

QtJson::JsonObject Player::model_gitem_action(const QtJson::JsonObject & command) {
    FIND_WIDGET_OR_RETURN(command, "oid", QGraphicsView, view, obj, id);
    QGraphicsItem * item = graphicsItemFromPath(view, command["stackpath"].toString());
    if (!item) {
        return createError("MissingModel", QString::fromUtf8("La vue (id:%1) ne possède pas de modèle.").arg(id));
    }
    view->ensureVisible(item); // pour rendre l'item visible
    QString itemaction = command["itemaction"].toString();

    QPoint viewPos = view->mapFromScene(item->mapToScene(item->boundingRect().center()));
    if (itemaction == "click") {
        if (view->scene() && view->scene()->mouseGrabberItem()) {
            view->scene()->mouseGrabberItem()->ungrabMouse();
        }
        mouse_click(view->viewport(), viewPos);
    } else if (itemaction == "doubleclick") {
        if (view->scene() && view->scene()->mouseGrabberItem()) {
            view->scene()->mouseGrabberItem()->ungrabMouse();
        }
        mouse_dclick(view->viewport(), viewPos);
    } else {
        return createError("MissingItemAction", QString::fromUtf8("itemaction %1 inconnue").arg(itemaction));
    }
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
        FIND_WIDGET_OR_RETURN(command, "oid", QWidget, _widget, obj, id);
        widget = _widget;
    } else {
        widget = qApp->activeWindow();
    }
    QString text = command["text"].toString();
    for (int i=0; i<text.count(); ++i) {
        QChar ch = text[i];
        int key = (int) ch.toAscii();
        qApp->postEvent(widget, new QKeyEvent(QKeyEvent::KeyPress, key, Qt::NoModifier, ch));
        qApp->postEvent(widget, new QKeyEvent(QKeyEvent::KeyRelease, key, Qt::NoModifier, ch));
    }
    QtJson::JsonObject result;
    return result;
}

QtJson::JsonObject Player::shortcut(const QtJson::JsonObject & command) {
    QWidget * widget;
    if (command.contains("oid")) {
        FIND_WIDGET_OR_RETURN(command, "oid", QWidget, _widget, obj, id);
        widget = _widget;
    } else {
        widget = qApp->activeWindow();
    }
    QKeySequence binding = QKeySequence::fromString(command["keysequence"].toString());

    // taken from
    // http://stackoverflow.com/questions/14283764/how-can-i-simulate-emission-of-a-standard-key-sequence
    for (uint i = 0; i < binding.count(); ++i) {
        uint key = binding[i];
        Qt::KeyboardModifiers modifiers = static_cast<Qt::KeyboardModifiers>(key & Qt::KeyboardModifierMask);
        key = key & ~Qt::KeyboardModifierMask;

        qApp->postEvent(widget, new QKeyEvent(QKeyEvent::KeyPress, key, modifiers));
        qApp->postEvent(widget, new QKeyEvent(QKeyEvent::KeyRelease, key, modifiers));
    }

    QtJson::JsonObject result;
    return result;
}

QtJson::JsonObject Player::tabbar_list(const QtJson::JsonObject & command) {
    FIND_WIDGET_OR_RETURN(command, "oid", QTabBar, tabbar, obj, id);
    QStringList texts;
    for (int i=0; i< tabbar->count(); ++i) {
        texts << tabbar->tabText(i);
    }
    QtJson::JsonObject result;
    result["tabtexts"] = texts;
    return result;
}

QtJson::JsonObject Player::graphicsitems(const QtJson::JsonObject & command) {
    FIND_WIDGET_OR_RETURN(command, "oid", QGraphicsView, view, obj, id);
    QList<QGraphicsItem *> topLevelItems;
    foreach(QGraphicsItem * item, view->items()) {
        if (! item->parentItem()) {
            topLevelItems << item;
        }
    }
     QtJson::JsonObject result;
     dump_graphics_items(topLevelItems, id, result);
     return result;
}

QtJson::JsonObject Player::gitem_properties(const QtJson::JsonObject & command) {
    FIND_WIDGET_OR_RETURN(command, "oid", QGraphicsView, view, obj, id);
    QString stackpath = command["stackpath"].toString();
    QGraphicsItem * item = graphicsItemFromPath(view, stackpath);
    if (!item) {
        return createError("MissingGItem", QString::fromUtf8("QGraphicsitem %1 introuvable sur la vue %2")
                           .arg(stackpath).arg(id));
    }
    QObject * object = dynamic_cast<QObject *>(item);
    if (!object) {
        return createError("GItemNotQObject", QString::fromUtf8("QGraphicsitem %1 sur la vue %2 n'hérite pas de QObject")
                           .arg(stackpath).arg(id));
    }
    QtJson::JsonObject result;
    dump_properties(object, result);
    return result;
}

void calculate_drag_n_drop_moves(QList<QPoint> & moves,
                                 const QPoint & globalSourcePos,
                                 const QPoint & globalDestPos,
                                 int deltaFactor=4) {
    QPoint delta = globalDestPos - globalSourcePos;
    delta /= deltaFactor;
    
    QPoint move = globalSourcePos;
    QPoint lastMove = globalSourcePos;
    for (int i = 0; i < deltaFactor; ++i) {
        move += delta;
        if (move != lastMove) {
            lastMove = move;
            moves << move;
        }
    }
    moves << globalDestPos;
}

void _drag_n_drop(QWidget * source, const QPoint & sourcePos,
                 QWidget * dest, const QPoint & destPos,
                 int startDragTime=0,
                 int deltaFactor=4) {
    QPoint globalSourcePos = source->mapToGlobal(sourcePos);
    QPoint globalDestPos = dest->mapToGlobal(destPos);
    
    // 1: press event
    qApp->postEvent(source,
        new QMouseEvent(QEvent::MouseButtonPress,
                        sourcePos,
                        globalSourcePos,
                        Qt::LeftButton,
                        Qt::NoButton,
                        Qt::NoModifier));
    
    // 2: wait drag time
    if (startDragTime == 0) {
        startDragTime = QApplication::startDragTime();
    }
    QEventLoop localLoop;
    QTimer::singleShot(startDragTime, &localLoop, SLOT(quit()));
    localLoop.exec();
    
    // 3: do some move event
    QList<QPoint> moves;
    calculate_drag_n_drop_moves(moves, globalSourcePos, globalDestPos, deltaFactor);
    foreach (const QPoint & move, moves) {
        QWidget * widgetUnderCursor = qApp->widgetAt(move);
        if (widgetUnderCursor) {
            qApp->postEvent(widgetUnderCursor,
                new QMouseEvent(QEvent::MouseMove,
                                widgetUnderCursor->mapFromGlobal(move),
                                move,
                                Qt::LeftButton,
                                Qt::NoButton,
                                Qt::NoModifier));
        }
    }
    
    // 4: now release the button
    qApp->postEvent(dest,
        new QMouseEvent(QEvent::MouseButtonRelease,
                        globalDestPos,
                        destPos,
                        Qt::LeftButton,
                        Qt::NoButton,
                        Qt::NoModifier));
}

QtJson::JsonObject Player::drag_n_drop(const QtJson::JsonObject & command) {
    FIND_WIDGET_OR_RETURN(command, "srcoid", QWidget, w1, obj1, id1);
    FIND_WIDGET_OR_RETURN(command, "destoid", QWidget, w2, obj2, id2);
    
    QPoint srcPos;
    if (command.contains("srcpos") && ! command["srcpos"].isNull()) {
        srcPos = pointFromString(command["srcpos"].toString());
    } else {
        srcPos = w1->rect().center();
    }
    
    QPoint destPos;
    if (command.contains("destpos") && ! command["destpos"].isNull()) {
        destPos = pointFromString(command["destpos"].toString());
    } else {
        destPos = w2->rect().center();
    }
    _drag_n_drop(w1, srcPos, w2, destPos);
    return QtJson::JsonObject();
}
