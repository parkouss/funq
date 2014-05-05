#ifndef DELAYEDRESPONSE_H
#define DELAYEDRESPONSE_H

#include <QObject>
#include "jsonclient.h"
#include <QTimer>

class DelayedResponse : public QObject {
    Q_OBJECT
public:
    explicit DelayedResponse(JsonClient * client, const QtJson::JsonObject & command, int interval=0);
    
    void setInterval(int interval) { m_timer.setInterval(interval); }

    void start();

protected:
    virtual void execute(const QtJson::JsonObject & command) = 0;
    void writeResponse(const QtJson::JsonObject & result);
    JsonClient * jsonClient() { return m_client; }

private slots:
    void timerCall();

private:

    JsonClient * m_client;
    QTimer m_timer;
    QtJson::JsonObject m_command;
    bool m_hasResponded;
};

#endif // DELAYEDRESPONSE_H
