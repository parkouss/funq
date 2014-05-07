#include "delayedresponse.h"
#include "protocole.h"
#include <QTimer>
#include <QDebug>

DelayedResponse::DelayedResponse(JsonClient * client, const QtJson::JsonObject & command, int interval, int timerOut) :
                                 QObject(client),
                                 m_client(client),
                                 m_command(command),
                                 m_hasResponded(false)
{
    Q_ASSERT(client);
    m_timer.setInterval(interval);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerCall()));

    QTimer::singleShot(timerOut, this, SLOT(onTimerOut()));
}

void DelayedResponse::start() {
    m_timer.start();
}

void DelayedResponse::timerCall() {
    if (! m_hasResponded) {
        execute(m_command);
    }
}

void DelayedResponse::onTimerOut() {
    if (! m_hasResponded) {
        writeResponse(jsonClient()->createError("DelayedResponseTimeOut",
                                                QString::fromUtf8("Délai de non réponse dépassé pour %2")
                                                .arg(staticMetaObject.className())));
    }
}

void DelayedResponse::writeResponse(const QtJson::JsonObject & result) {
    m_timer.stop();
    emit aboutToWriteResponse();
    m_hasResponded = true;

    bool success;
    QByteArray response = QtJson::serialize(result, success);

    if (!success) {
        qDebug() << "unable to serialize result to json" << m_command["action"].toString();
        m_client->protocole()->close();
        return;
    }
    
    m_client->protocole()->sendMessage(response);
}
