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

#include "dragndrophelpers.h"
#include <QPoint>
#include <QList>
#include <QString>

void calculate_drag_n_drop_moves(QList<QPoint> & moves,
                                 const QPoint & globalSourcePos,
                                 const QPoint & globalDestPos,
                                 int deltaFactor) {
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
