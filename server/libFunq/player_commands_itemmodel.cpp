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

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QTableView>
#include <QTreeView>
#include <QHeaderView>

using namespace ObjectPath;

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
            if (j == 0 && recursive && model->hasChildren(index)) {
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

/* player commands */

QtJson::JsonObject Player::model_items(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QAbstractItemView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QtJson::JsonObject result;
    QAbstractItemModel * model = ctx.widget->model();
    if (!model) {
        return createError("MissingModel", QString::fromUtf8("The view (id:%1) has no associated model").arg(ctx.id));
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
        return createError("MissingModel", QString::fromUtf8("The view (id:%1) has no associated model").arg(ctx.id));
    }
    QModelIndex index = get_model_item(model, command["itempath"].toString(), command["row"].toInt(), command["column"].toInt());
    if (!index.isValid()) {
        return createError("MissingModelItem", QString::fromUtf8("Unable to find an item identified by %1").arg(command["itempath"].toString()));
    }
    ctx.widget->scrollTo(index); // item visible
    QString itemaction = command["itemaction"].toString();

    QPoint cursorPosition;

    if (itemaction == "click" || itemaction == "doubleclick") {
        QString origin = command["origin"].toString();
        int offsetX = command["offset_x"].toInt();
        int offsetY = command["offset_y"].toInt();
        QRect visualRect = ctx.widget->visualRect(index);
        cursorPosition = visualRect.center();
        if (origin == "left") {
            cursorPosition.setX(visualRect.x());
        } else if (origin == "right") {
            cursorPosition.setX(visualRect.width() - 1);
        }
        int newX = cursorPosition.x() + offsetX;
        int newY = cursorPosition.y() + offsetY;

        /* The new coordinates have to be within the bounds */
        if (newX < visualRect.x())
            newX = visualRect.x() + 2;
        else if (newX > visualRect.x() + visualRect.width())
            newX = visualRect.x() + visualRect.width() - 2;

        if (newY < visualRect.y())
            newY = visualRect.y() + 2;
        else if (newY > visualRect.y() + visualRect.height())
            newY = visualRect.y() + visualRect.height() - 2;

        cursorPosition.setX(newX);
        cursorPosition.setY(newY);
    }

    if (itemaction == "select") {
        emit emit_model_item_action(itemaction, ctx.widget, index);
    } else if (itemaction == "edit") {
        emit emit_model_item_action(itemaction, ctx.widget, index);
    } else if (itemaction == "click") {
        mouse_click(ctx.widget->viewport(), cursorPosition);
    } else if (itemaction == "doubleclick") {
        mouse_dclick(ctx.widget->viewport(), cursorPosition);
    } else {
        return createError("MissingItemAction", QString::fromUtf8("itemaction %1 unknown").arg(itemaction));
    }
    QtJson::JsonObject result;
    return result;
}

void Player::_model_item_action(const QString & action, QAbstractItemView * widget, const QModelIndex & index) {
    if (action == "select") {
        widget->setCurrentIndex(index);
    } else if (action == "edit") {
        widget->setCurrentIndex(index);
        widget->edit(index);
    }
}

QtJson::JsonObject Player::headerview_path_from_view(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QAbstractItemView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }

    QHeaderView * header = NULL;
    QTableView * tview = qobject_cast<QTableView *>(ctx.widget);
    if (tview) {
        if (command["orientation"] == "vertical") {
            header = tview->verticalHeader();
        } else {
            header = tview->horizontalHeader();
        }
    } else {
        QTreeView * trview = qobject_cast<QTreeView *>(ctx.widget);
        if (trview) {
            header = trview->header();
        }
    }

    if (! header) {
        return createError("InvalidHeaderView", QString::fromUtf8("No header view found for the view (id:%1)").arg(ctx.id));
    }
    QtJson::JsonObject result;
    result["headerpath"] = objectPath(header);
    return result;
}

QtJson::JsonObject Player::headerview_list(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QHeaderView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    QAbstractItemModel * model = ctx.widget->model();
    if (!model) {
        return createError("MissingModel", QString::fromUtf8("The header view (id:%1) has no associated model").arg(ctx.id));
    }
    QStringList texts;
    int nbItems = ctx.widget->orientation() == Qt::Vertical ? model->rowCount() : model->columnCount();
    for (int i=0; i<nbItems; i++) {
        texts << model->headerData(i, ctx.widget->orientation()).toString();
    }
    QtJson::JsonObject result;
    result["headertexts"] = texts;
    return result;
}

QtJson::JsonObject Player::headerview_click(const QtJson::JsonObject & command) {
    WidgetLocatorContext<QHeaderView> ctx(this, command, "oid");
    if (ctx.hasError()) { return ctx.lastError; }
    int logicalIndex;
    QVariant indexOrName = command["indexOrName"];
    if (indexOrName.type() == QVariant::String) {
        QString name = indexOrName.toString();
        QAbstractItemModel * model = ctx.widget->model();
        if (!model) {
            return createError("MissingModel", QString::fromUtf8("The header view (id:%1) has no associated model").arg(ctx.id));
        }
        bool found = false;
        int nbItems = ctx.widget->orientation() == Qt::Horizontal ? model->rowCount() : model->columnCount();
        for (int i=0; i<nbItems; i++) {
            if (name == model->headerData(i, ctx.widget->orientation()).toString()) {
                logicalIndex = i;
                found = true;
                break;
            }
        }
        if (!found) {
            return createError("MissingHeaderViewText", QString::fromUtf8("The header view (id:%1) has no text column `%2`").arg(ctx.id).arg(name));
        }
    } else {
        logicalIndex = ctx.widget->logicalIndex(command["indexOrName"].toInt());
    }

    int pos = ctx.widget->sectionPosition(logicalIndex);
    if (pos == -1) {
        return createError("InvalidHeaderViewIndex", QString::fromUtf8("The header view (id:%1) has no index %2 or it is hidden").arg(ctx.id).arg(logicalIndex));
    }
    QPoint mousePos;
    if (ctx.widget->orientation() == Qt::Horizontal) {
        mousePos.setY(ctx.widget->height()/2);
        mousePos.setX(pos + ctx.widget->offset() + 5);
    } else {
        mousePos.setX(ctx.widget->width()/2);
        mousePos.setY(pos + ctx.widget->offset() + 5);
    }
    mouse_click(ctx.widget->viewport(), mousePos);
    QtJson::JsonObject result;
    return result;
}
