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

#ifndef PLAYER_UTILS_H
#define PLAYER_UTILS_H

#include <QPoint>
#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#ifdef QT_QUICK_LIB
#include <QWindow>
#endif
#include "player.h"
#include "jsonclient.h"

template<class T>
void mouse_click(T * w, const QPoint & pos) {
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

template<class T>
void mouse_dclick(T * w, const QPoint & pos) {
    mouse_click(w, pos);
    qApp->postEvent(w,
        new QMouseEvent(QEvent::MouseButtonDblClick,
                        pos,
                        w->mapToGlobal(pos),
                        Qt::LeftButton,
                        Qt::NoButton,
                        Qt::NoModifier));
}

void activate_focus(QWidget * w);

void dump_properties(QObject * object, QtJson::JsonObject & out);

void dump_object(QObject * object, QtJson::JsonObject & out, bool with_properties = false);

/**
  * @brief Allow to find a previously referenced object.
  */
class ObjectLocatorContext {
public:
    ObjectLocatorContext(Player * player,
                         const QtJson::JsonObject & command,
                         const QString & objKey);
    virtual ~ObjectLocatorContext() {}

    qulonglong id;
    QObject * obj;
    QtJson::JsonObject lastError;
    inline bool hasError() { return ! lastError.isEmpty(); }
};

/**
  * @brief Allow to find a previously referenced widget (with type T).
  */
template <class T = QWidget>
class WidgetLocatorContext : public ObjectLocatorContext {
public:
    WidgetLocatorContext(Player * player,
                         const QtJson::JsonObject & command,
                         const QString & objKey) : ObjectLocatorContext(player, command, objKey) {

        if (! hasError()) {
            widget = qobject_cast<T *>(obj);
            if (!widget) {
                lastError = player->createError("NotAWidget",
                                                QString::fromUtf8("Object (id:%1) is not a %2").arg(id).arg(T::staticMetaObject.className()));
            }
        }
    }
    T * widget;
};

#endif // PLAYER_UTILS_H
