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
    DelayedResponse(client, command),
    m_state(Press)
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

void DragNDropResponse::execute(const QtJson::JsonObject &) {
    switch (m_state) {
    case Press:
        m_srcPosGlobal = m_src->mapToGlobal(m_srcPos);
        m_destPosGlobal = m_dest->mapToGlobal(m_destPos);

        // 1: press event
        qApp->postEvent(m_src,
            new QMouseEvent(QEvent::MouseButtonPress,
                            m_srcPos,
                            m_srcPosGlobal,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier));
        m_state = WaitForDragStart;
        break;
    case WaitForDragStart: {
        setInterval(qApp->startDragTime());
        m_state = Move;
        break;
    }
    case Move: {
        setInterval(0);
        // 3: do some move event
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
        m_state = Release;
        break;
    }
    case Release: {
        // 4: now release the button
        qApp->postEvent(m_dest,
            new QMouseEvent(QEvent::MouseButtonRelease,
                            m_destPos,
                            m_destPosGlobal,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier));
        writeResponse(QtJson::JsonObject());
        break;
    }
    }
}
