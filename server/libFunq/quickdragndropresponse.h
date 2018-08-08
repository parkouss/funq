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

#ifndef QUICKDRAGNDROPRESPONSE_H
#define QUICKDRAGNDROPRESPONSE_H

#include "delayedresponse.h"
#include <QQuickItem>
#include <QQuickWindow>

class QuickDragNDropResponse : public DelayedResponse
{
public:
    explicit QuickDragNDropResponse(JsonClient * client, const QtJson::JsonObject & command);

protected:
    virtual void execute(int call);

signals:

public slots:

private:
    QQuickWindow * m_window;
    QPoint m_srcPosGlobal;
    QPoint m_destPosGlobal;
    QList<QPoint> m_moves;
};

#endif // QUICKDRAGNDROPRESPONSE_H
