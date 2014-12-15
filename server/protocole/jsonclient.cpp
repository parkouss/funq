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

#include "jsonclient.h"
#include "protocole.h"
#include "delayedresponse.h"
#include <QDebug>
#include <QMetaMethod>

JsonClient::JsonClient(QIODevice *device, QObject *parent) :
    QObject(parent), m_protocole(new Protocole(this))
{
    m_protocole->setDevice(device);
    connect(m_protocole, SIGNAL(messageReceived()), this, SLOT(onMessageReceived()));
}

JsonClient::~JsonClient() {
    delete m_protocole;
}

void JsonClient::onMessageReceived() {
    QByteArray data = m_protocole->nextAvailableMessage();
    bool success = false;
    QVariant message = QtJson::parse(data, success);
    if (!success) {
        qDebug() << "Unable to parse Json data. received:";
        qDebug() << data;
        m_protocole->close();
        return;
    }

    QtJson::JsonObject command = message.value<QtJson::JsonObject>();
    if (!command.contains("action")) {
        qDebug() << "a JSon object is required with an 'action' field";
        m_protocole->close();
        return;
    }

    QString action = command["action"].toString();
    // localise la méthode
    success = false;
    QMetaMethod method;
    QByteArray action_str = action.toLatin1();
    size_t len_signature = strlen(action_str.data());
    for (int i = metaObject()->methodOffset(); i < metaObject()->methodCount(); ++i) {
        method = metaObject()->method(i);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        QString signature = method.methodSignature();
#else
        QString signature = method.signature();
#endif
        if (    signature.startsWith(action) &&
                signature.at(action.length()) == '(') {
            success = true;
            break;
        }
    }
    if (!success) {
        qDebug() << "unable to find action" << action;
        m_protocole->close();
        return;
    }
    
    bool is_delayed_call = ! (strcmp(method.typeName(), "QtJson::JsonObject") == 0);
    
    if (! is_delayed_call) {
        QtJson::JsonObject result;
        success = method.invoke(this, Qt::DirectConnection,
                                      Q_RETURN_ARG(QtJson::JsonObject, result),
                                      Q_ARG(QtJson::JsonObject, command));
        if (!success) {
            qDebug() << "error while executing action" << action;
            m_protocole->close();
            return;
        }

        // serialize response
        QByteArray response = QtJson::serialize(result, success);

        if (!success) {
            qDebug() << "unable to serialize result to json" << action;
            m_protocole->close();
            return;
        }

        m_protocole->sendMessage(response);
    } else {
        DelayedResponse * dresponse;
        success = method.invoke(this, Qt::DirectConnection,
                                      Q_RETURN_ARG(DelayedResponse *, dresponse),
                                      Q_ARG(QtJson::JsonObject, command));
        if (!success && dresponse) {
            qDebug() << "error while executing action" << action;
            m_protocole->close();
            return;
        }
        connect(dresponse, SIGNAL(aboutToWriteResponse(const QtJson::JsonObject &)), dresponse, SLOT(deleteLater()));
        dresponse->start();
    }
}

QtJson::JsonObject JsonClient::createError(const QString &name, const QString &description) {
    QtJson::JsonObject message;
    message["success"] = false;
    message["errName"] = name;
    message["errDesc"] = description;
    return message;
}
