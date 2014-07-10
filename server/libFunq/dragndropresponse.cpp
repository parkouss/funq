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

#include "dragndropresponse.h"
#include <QApplication>
#include <QMouseEvent>
#include <QTimer>
#include "player.h"

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

QPoint pointFromString(const QString & data) {
    QStringList splitted = data.split(",");
    if (splitted.count() == 2) {
        QPoint p;
        p.setX(splitted[0].toInt());
        p.setY(splitted[1].toInt());
        return p;
    }
    return QPoint();
}

DragNDropResponse::DragNDropResponse(JsonClient *client, const QtJson::JsonObject &command) :
    DelayedResponse(client, command)
{
    WidgetLocatorContext<QWidget> ctx(static_cast<Player *>(jsonClient()), command, "srcoid");
    WidgetLocatorContext<QWidget> ctx2(static_cast<Player *>(jsonClient()), command, "destoid");

    if (ctx.hasError()) { writeResponse(ctx.lastError); return; }
    if (ctx2.hasError()) { writeResponse(ctx2.lastError); return; }

    QPoint srcPos;
    if (command.contains("srcpos") && ! command["srcpos"].isNull()) {
        srcPos = pointFromString(command["srcpos"].toString());
    } else {
        srcPos = ctx.widget->rect().center();
    }

    QPoint destPos;
    if (command.contains("destpos") && ! command["destpos"].isNull()) {
        destPos = pointFromString(command["destpos"].toString());
    } else {
        destPos = ctx2.widget->rect().center();
    }

    m_src = ctx.widget;
    m_dest = ctx2.widget;
    m_srcPos = srcPos;
    m_destPos = destPos;
}

void DragNDropResponse::execute(int call) {
    switch (call) {
    case 0: // pre-phase : ensure widgets are painted in order to mapToGlobal to work
        m_src->repaint ();
        m_dest->repaint ();
        setInterval(100);
        break;
    case 1: // 1: press event
        m_srcPosGlobal = m_src->mapToGlobal(m_srcPos);
        m_destPosGlobal = m_dest->mapToGlobal(m_destPos);

        qApp->postEvent(m_src,
            new QMouseEvent(QEvent::MouseButtonPress,
                            m_srcPos,
                            m_srcPosGlobal,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier));
        break;
    case 2: { // 2: WaitForDragStart
        setInterval(qApp->startDragTime() + 20);
        break;
    }
    case 3: { // 3: do some move event
        setInterval(0);
        QList<QPoint> moves;
        calculate_drag_n_drop_moves(moves, m_srcPosGlobal, m_destPosGlobal, 4);
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
        break;
    }
    case 4: { // 4: now release the button
        qApp->postEvent(m_dest,
            new QMouseEvent(QEvent::MouseButtonRelease,
                            m_destPos,
                            m_destPosGlobal,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier));
    }
    case 5: // and reply
        writeResponse(QtJson::JsonObject());
        break;
    }
}
