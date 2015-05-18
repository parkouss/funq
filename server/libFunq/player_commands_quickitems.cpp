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

#ifdef QT_QUICK_LIB
#include <QWindow>
#include <QQuickWindow>
#include <QQuickItem>
#endif

using namespace ObjectPath;


#ifdef QT_QUICK_LIB
class QuickItemLocatorContext : public ObjectLocatorContext {
public:
    QuickItemLocatorContext(Player * player,
                            const QtJson::JsonObject & command,
                            const QString & objKey);
    QQuickItem * item;
    QQuickWindow * window;
};

QuickItemLocatorContext::QuickItemLocatorContext(Player * player,
                                                 const QtJson::JsonObject & command,
                                                 const QString & objKey) : ObjectLocatorContext(player, command, objKey) {
    if (! hasError()) {
        item = qobject_cast<QQuickItem *>(obj);
        if (!item) {
            lastError = player->createError("NotAWidget",
                                            QString::fromUtf8("Object (id:%1) is not a QQuickItem").arg(id));
        } else {
            window = item->window();
            if (! window) {
                lastError = player->createError("NoWindowForQuickItem", "No QQuickWindow associated to the item.");
            }
        }
    }
}
#endif


QtJson::JsonObject Player::quick_item_find(const QtJson::JsonObject & command) {
    QtJson::JsonObject result;
#ifdef QT_QUICK_LIB
    WidgetLocatorContext<QQuickWindow> ctx(this, command, "quick_window_oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QQuickItem * item;
    qulonglong id;
    QString qid = command["qid"].toString();
    if (! qid.isEmpty()) {
        item = ObjectPath::findQuickItemById(ctx.widget->contentItem(), qid);
        id = registerObject(item);
        if (id == 0) {
            return createError("InvalidQuickItem", QString("Unable to find quick item with id `%1`").arg(qid));
        }
    } else {
        QString path = command["path"].toString();
        item = ObjectPath::findQuickItem(ctx.widget, path);
        id = registerObject(item);
        if (id == 0) {
            return createError("InvalidQuickItem", QString("Unable to find quick item with path `%1`").arg(path));
        }
    }
    result["oid"] = id;
    result["quick_window_oid"] = command["quick_window_oid"].toString();
    dump_object(item, result);
#else
    result = createQtQuickOnlyError();
#endif
    return result;
}

QtJson::JsonObject Player::quick_item_click(const QtJson::JsonObject & command) {
#ifdef QT_QUICK_LIB
    QuickItemLocatorContext ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }

    QPoint sPos = ctx.item->mapToScene(QPointF(0,0)).toPoint();
    sPos.rx() += ctx.item->width() / 2;
    sPos.ry() += ctx.item->height() / 2;
    mouse_click(ctx.window, sPos);
    QtJson::JsonObject result;
    return result;
#else
    return createQtQuickOnlyError();
#endif
}
