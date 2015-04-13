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

#include "delayedresponse.h"
#include "protocole.h"
#include <QTimer>
#include <QDebug>

DelayedResponse::DelayedResponse(JsonClient * client, const QtJson::JsonObject & command, int interval, int timerOut) :
                                 QObject(client),
                                 m_client(client),
                                 m_hasResponded(false),
                                 m_nbCall(0)
{
    Q_ASSERT(client);
    m_timer.setInterval(interval);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerCall()));

    QTimer::singleShot(timerOut, this, SLOT(onTimerOut()));
    
    m_action = command["action"].toString();
}

void DelayedResponse::start() {
    m_timer.start();
}

void DelayedResponse::timerCall() {
    if (! m_hasResponded) {
        execute(m_nbCall);
        m_nbCall += 1;
    }
}

void DelayedResponse::onTimerOut() {
    if (! m_hasResponded) {
        writeResponse(jsonClient()->createError("DelayedResponseTimeOut",
                                                QString::fromUtf8("Timeout for non response: %2")
                                                .arg(staticMetaObject.className())));
    }
}

void DelayedResponse::writeResponse(const QtJson::JsonObject & result) {
    m_timer.stop();
    emit aboutToWriteResponse(result);
    m_hasResponded = true;

    bool success;
    QByteArray response = QtJson::serialize(result, success);

    if (!success) {
        qDebug() << "unable to serialize result to json" << m_action;
        m_client->protocole()->close();
        return;
    }
    
    m_client->protocole()->sendMessage(response);
}
