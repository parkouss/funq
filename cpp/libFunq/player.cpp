#include "player.h"
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
#include <QTime>
#include "dragndropresponse.h"
#include "shortcutresponse.h"

using namespace ObjectPath;

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
        // sometimes classes appears twice
        if (!classes.contains(mo->className())) {
            classes << mo->className();
        }
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

QtJson::JsonObject Player::active_window(const QtJson::JsonObject &) {
    QWidget * activewindow = QApplication::activeWindow();
    if (! activewindow) {
        return createError("NoActiveWindow", QString::fromUtf8("Il n'y a pas de fenêtre active"));
    }
    qulonglong id = registerObject(activewindow);
    QtJson::JsonObject result;
    result["oid"] = id;
    dump_object(activewindow, result);
    return result;
}

ObjectLocatorContext::ObjectLocatorContext(Player * player,
                                           const QtJson::JsonObject & command,
                                           const QString & oidKey) {
    id = command[oidKey].value<qulonglong>();
    obj = player->registeredObject(id);
    if (!obj) {
        lastError = player->createError("NotRegisteredObject",
                           QString::fromUtf8("L'objet (id:%1) n'est pas enregistré ou a été détruit").arg(id));
    }
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
    QtJson::JsonObject properties = command["properties"].value<QtJson::JsonObject>();
    QTimer::singleShot(0, this, SLOT(object_set_properties(QObject *, const QtJson::JsonObject &)));
    QtJson::JsonObject result;
    return result;
}

void Player::object_set_properties(QObject * object, const QtJson::JsonObject & properties) {
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
        foreach (QWidget * widget, QApplication::topLevelWidgets()) {
            recursive_list_widget(widget, result, with_properties);
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

QtJson::JsonObject Player::model_items(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QAbstractItemView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QtJson::JsonObject result;
    QAbstractItemModel * model = ctx.widget->model();
    if (!model) {
        return createError("MissingModel", QString::fromUtf8("La vue (id:%1) ne possède pas de modèle.").arg(ctx.id));
    }
    bool recursive = ! (model->inherits("QTableModel") || ctx.widget->inherits("QTableView") || ctx.widget->inherits("QListView"));
    dump_items_model(model, result, QModelIndex(), ctx.id, recursive);
    return result;
}

QtJson::JsonObject Player::model_item_action(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QAbstractItemView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QAbstractItemModel * model = ctx.widget->model();
    if (!model) {
        return createError("MissingModel", QString::fromUtf8("La vue (id:%1) ne possède pas de modèle.").arg(ctx.id));
    }
    QModelIndex index = get_model_item(model, command["itempath"].toString(), command["row"].toInt(), command["column"].toInt());
    if (!index.isValid()) {
        return createError("MissingModelItem", QString::fromUtf8("Impossible de trouver l'item identifié par %1").arg(command["itempath"].toString()));
    }
    ctx.widget->scrollTo(index); // item visible
    QString itemaction = command["itemaction"].toString();
    if (itemaction == "select") {
        ctx.widget->setCurrentIndex(index);
    } else if (itemaction == "edit") {
        ctx.widget->setCurrentIndex(index);
        ctx.widget->edit(index);
    } else if (itemaction == "click") {
        QRect visualRect = ctx.widget->visualRect(index);
        mouse_click(ctx.widget->viewport(), visualRect.center());
    } else if (itemaction == "doubleclick") {
        QRect visualRect = ctx.widget->visualRect(index);
        mouse_dclick(ctx.widget->viewport(), visualRect.center());
    } else {
        return createError("MissingItemAction", QString::fromUtf8("itemaction %1 inconnue").arg(itemaction));
    }
    QtJson::JsonObject result;
    return result;
}

QtJson::JsonObject Player::model_gitem_action(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QGraphicsView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QGraphicsItem * item = graphicsItemFromPath(ctx.widget, command["stackpath"].toString());
    if (!item) {
        return createError("MissingModel", QString::fromUtf8("La vue (id:%1) ne possède pas de modèle.").arg(ctx.id));
    }
    ctx.widget->ensureVisible(item); // pour rendre l'item visible
    QString itemaction = command["itemaction"].toString();

    QPoint viewPos = ctx.widget->mapFromScene(item->mapToScene(item->boundingRect().center()));
    if (itemaction == "click") {
        if (ctx.widget->scene() && ctx.widget->scene()->mouseGrabberItem()) {
            ctx.widget->scene()->mouseGrabberItem()->ungrabMouse();
        }
        mouse_click(ctx.widget->viewport(), viewPos);
    } else if (itemaction == "doubleclick") {
        if (ctx.widget->scene() && ctx.widget->scene()->mouseGrabberItem()) {
            ctx.widget->scene()->mouseGrabberItem()->ungrabMouse();
        }
        mouse_dclick(ctx.widget->viewport(), viewPos);
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
        WidgetLocatorContext<QWidget> ctx(this, command, "oid");
        if (ctx.hasError()) { return ctx.lastError; }
        widget = ctx.widget;
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

QtJson::JsonObject Player::graphicsitems(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QGraphicsView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QList<QGraphicsItem *> topLevelItems;
    foreach(QGraphicsItem * item, ctx.widget->items()) {
        if (! item->parentItem()) {
            topLevelItems << item;
        }
    }
     QtJson::JsonObject result;
     dump_graphics_items(topLevelItems, ctx.id, result);
     return result;
}

QtJson::JsonObject Player::gitem_properties(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QGraphicsView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QString stackpath = command["stackpath"].toString();
    QGraphicsItem * item = graphicsItemFromPath(ctx.widget, stackpath);
    if (!item) {
        return createError("MissingGItem", QString::fromUtf8("QGraphicsitem %1 introuvable sur la vue %2")
                           .arg(stackpath).arg(ctx.id));
    }
    QObject * object = dynamic_cast<QObject *>(item);
    if (!object) {
        return createError("GItemNotQObject", QString::fromUtf8("QGraphicsitem %1 sur la vue %2 n'hérite pas de QObject")
                           .arg(stackpath).arg(ctx.id));
    }
    QtJson::JsonObject result;
    dump_properties(object, result);
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
        return createError("NoMethodInvoked", QString::fromUtf8("Le slot %1 n'a pas pu être appelé")
                           .arg(slot_name));
    }

    QtJson::JsonObject result;
    result["result_slot"] = result_slot;
    return result;
}
