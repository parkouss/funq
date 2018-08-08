/*
Copyright: SCLE SFE
Contributor: Julien Pagès <j.parkouss@gmail.com>
Contributor: Anthony Léonard <anthony.leonard@savoirfairelinux.com>

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

#include "quickdragndropresponse.h"
#include "dragndrophelpers.h"
#include <QApplication>
#include <QMouseEvent>
#include <QTimer>
#include "player.h"

QuickDragNDropResponse::QuickDragNDropResponse(JsonClient *client, const QtJson::JsonObject &command) :
    DelayedResponse(client, command)
{
    QuickItemLocatorContext ctx(static_cast<Player *>(jsonClient()), command, "srcoid");
    QuickItemLocatorContext ctx2(static_cast<Player *>(jsonClient()), command, "destoid");

    if (ctx.hasError()) { writeResponse(ctx.lastError); return; }
    if (ctx2.hasError()) { writeResponse(ctx2.lastError); return; }

    QPoint srcPos;
    if (command.contains("srcpos") && ! command["srcpos"].isNull()) {
        srcPos = pointFromString(command["srcpos"].toString());
    } else {
        srcPos = QPoint(ctx.item->width() / 2.0, ctx.item->height() / 2.0);
    }

    QPoint destPos;
    if (command.contains("destpos") && ! command["destpos"].isNull()) {
        destPos = pointFromString(command["destpos"].toString());
    } else {
        destPos = QPoint(ctx2.item->width() / 2.0, ctx2.item->height() / 2.0);
    }

    m_window = ctx.window;
    m_srcPosGlobal = ctx.item->mapToGlobal(srcPos).toPoint();
    m_destPosGlobal = ctx2.item->mapToGlobal(destPos).toPoint();
    calculate_drag_n_drop_moves(m_moves, m_srcPosGlobal, m_destPosGlobal, 8);
}

void QuickDragNDropResponse::execute(int call) {
    switch (call) {
    case 0: // 0: press event
        qApp->postEvent(m_window,
            new QMouseEvent(QEvent::MouseButtonPress,
                            m_window->mapFromGlobal(m_srcPosGlobal),
                            m_srcPosGlobal,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier));
        setInterval(20);
        break;
    case 1: { // 1: do some move event
        if (m_moves.isEmpty())
            break;

        const QPoint move = m_moves.takeFirst();
        qApp->postEvent(m_window,
            new QMouseEvent(QEvent::MouseMove,
                            m_window->mapFromGlobal(move),
                            move,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier));
        m_nbCall -= 1;
        break;
    }
    case 2: { // 2: now release the button
        qApp->postEvent(m_window,
            new QMouseEvent(QEvent::MouseButtonRelease,
                            m_window->mapFromGlobal(m_destPosGlobal),
                            m_destPosGlobal,
                            Qt::LeftButton,
                            Qt::NoButton,
                            Qt::NoModifier));
    }
    case 3: // and reply
        writeResponse(QtJson::JsonObject());
        break;
    }
}
