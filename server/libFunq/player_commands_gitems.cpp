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

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QBuffer>

using namespace ObjectPath;

void dump_graphics_items(const QList<QGraphicsItem *>  & items, const qulonglong & viewid, QtJson::JsonObject & out) {
    QtJson::JsonArray outitems;
    foreach (QGraphicsItem * item, items) {
        QtJson::JsonObject outitem;
        outitem["gid"] = graphicsItemId(item);
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
        #if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        dump_graphics_items(item->childItems(), viewid, outitem);
        #else
        dump_graphics_items(item->children(), viewid, outitem);
        #endif
        outitems << outitem;
    }
    out["items"] = outitems;
}

QtJson::JsonObject Player::model_gitem_action(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QGraphicsView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    qulonglong gid = command["gid"].value<qulonglong>();
    QGraphicsItem * item = graphicsItemFromId(ctx.widget, gid);
    if (!item) {
        return createError("MissingGItem", QString::fromUtf8("The view (id:%1) has no associated item %2").arg(ctx.id).arg(gid));
    }
    ctx.widget->ensureVisible(item); // be sure item is visible
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
        return createError("MissingItemAction", QString::fromUtf8("itemaction %1 unknown").arg(itemaction));
    }
    QtJson::JsonObject result;
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
    qulonglong gid = command["gid"].value<qulonglong>();
    QGraphicsItem * item = graphicsItemFromId(ctx.widget, gid);
    if (!item) {
        return createError("MissingGItem", QString::fromUtf8("QGraphicsitem %1 is not in view %2")
                           .arg(gid).arg(ctx.id));
    }
    QObject * object = dynamic_cast<QObject *>(item);
    if (!object) {
        return createError("GItemNotQObject", QString::fromUtf8("QGraphicsitem %1 in view %2 does not inherit from QObject")
                           .arg(gid).arg(ctx.id));
    }
    QtJson::JsonObject result;
    dump_properties(object, result);
    return result;
}

QtJson::JsonObject Player::grab_graphics_view(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QGraphicsView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QString format = command["format"].toString();
    if (format.isEmpty()) {
        format = "PNG";
    }
    QPixmap pixmap(ctx.widget->scene()->width(), ctx.widget->scene()->height());
    QPainter q_painter(&pixmap);

    ctx.widget->scene()->render(&q_painter);
    QBuffer buffer;
    pixmap.save(&buffer, format.toStdString().c_str());

    QtJson::JsonObject result;
    result["format"] = format;
    result["data"] = buffer.data().toBase64();

    return result;
}
