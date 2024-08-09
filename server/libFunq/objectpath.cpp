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

#include "objectpath.h"

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QWidget>
#include <QWindow>

#ifdef QT_QUICK_LIB
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#endif

/**
 * Returns the object name (not unique given its siblings)
 */
inline QString _rawObjectName(QObject * object) {
    QString rawName = object->objectName();

    if (rawName.isEmpty()) {
        rawName = object->metaObject()->className();
    }
    return rawName;
}

/**
 * Returns the object name (unique given its siblings)
 */
QString rawObjectName(QObject * object) {
    QString rawName = _rawObjectName(object);

    if (!object->parent()) {
        return rawName;
    }

    const QList<QObject *> siblings = object->parent()->children();

    int index = 0;
    Q_FOREACH (QObject * sibling, siblings) {
        if (sibling == object) {
            break;
        }
        QString siblingName = _rawObjectName(sibling);
        if (siblingName == rawName) {
            ++index;
        }
    }
    if (index == 0) {
        return rawName;
    }
    return QString("%1-%2").arg(rawName).arg(index);
}

QString ObjectPath::objectPath(QObject * object) {
    QStringList components;
    QObject * current = object;
    while (current) {
        components.prepend(ObjectPath::objectName(current));
        current = current->parent();
    }
    return components.join("::");
}

QString ObjectPath::objectName(QObject * object) {
    QString name = rawObjectName(object);
    name.replace("::", ":_:");  // we use :: as path separators
    return name;
}

QObject * ObjectPath::findObject(const QString & path) {
    const QString separator("::");
    QStringList parts = path.split(separator);
    if (parts.isEmpty()) {
        return 0;
    }
    const QString name = parts.takeLast();
    QObject * parent = 0;
    if (parts.isEmpty()) {
        // Top level widget
        Q_FOREACH (QWidget * widget, QApplication::topLevelWidgets()) {
            if (objectName(widget) == name) {
                return widget;
            }
        }
        // did not find any ? - let's try on windows (qtquick)
        Q_FOREACH (QWindow * window, QApplication::topLevelWindows()) {
            if (objectName(window) == name) {
                return window;
            }
        }
        return 0;
    } else {
        parent = findObject(parts.join(separator));
        if (!parent) {
            return 0;
        }
    }

    Q_FOREACH (QObject * child, parent->children()) {
        if (objectName(child) == name) {
            return child;
        }
    }

    return 0;
}

qulonglong ObjectPath::graphicsItemId(QGraphicsItem * item) {
    return (qulonglong)item;
}

QGraphicsItem * ObjectPath::graphicsItemFromId(QGraphicsView * view,
                                               const qulonglong & id) {
    foreach (QGraphicsItem * item, view->items()) {
        if (((qulonglong)item) == id) {
            return item;
        }
    }
    return NULL;
}

/* quick items stuff */

#ifdef QT_QUICK_LIB
QString quickItemPath(QQuickItem * item) {
    QStringList components;
    QQuickItem * current = item;
    while (current) {
        components.prepend(ObjectPath::objectName(current));
        current = current->parentItem();
    }
    return components.join("::");
}

QQuickItem * ObjectPath::findQuickItem(QQuickWindow * window,
                                       const QString & path) {
    QStringList lstpath = path.split("::");
    if (lstpath.isEmpty() || !window) {
        return NULL;
    }

    QQuickItem * root = window->contentItem();

    while (root && !lstpath.isEmpty()) {
        QString itemName = lstpath.first();
        lstpath.removeFirst();
        bool find = false;
        foreach (QQuickItem * child, root->childItems()) {
            if (ObjectPath::objectName(child) == itemName) {
                find = true;
                root = child;
                break;
            }
        }
        if (!find) {
            return NULL;
        }
    }
    return root;
}

QQuickItem * ObjectPath::findQuickItemById(QQuickItem * root,
                                           const QString & qid) {
    QStringList qids = qid.split(".");
    if (qids.isEmpty()) {
        return NULL;
    }
    QList<QQuickItem *> items;
    items << root;

    while (!items.isEmpty()) {
        QQuickItem * item = items.first();
        items.removeFirst();
        QQmlContext * ctx = QQmlEngine::contextForObject(item);
        if (ctx && ctx->nameForObject(item) == qids.first()) {
            qids.removeFirst();
            if (qids.isEmpty()) {
                return item;
            }
            items.clear();
        }
        items += item->childItems();
    }
    return NULL;
}

#endif  // quick item stuff
