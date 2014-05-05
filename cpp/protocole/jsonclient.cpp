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
    const char * action_str = action.toAscii().data();
    size_t len_signature = strlen(action_str);
    for (int i = metaObject()->methodOffset(); i < metaObject()->methodCount(); ++i) {
        method = metaObject()->method(i);
        const char * signature = method.signature();
        if ((strncmp(action_str, signature, len_signature) == 0)
                        && (signature[len_signature] == '(')) {
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
