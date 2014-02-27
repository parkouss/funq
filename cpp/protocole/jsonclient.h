#ifndef JSONCLIENT_H
#define JSONCLIENT_H

#include <QObject>
#include "json.h"

class Protocole;
class QIODevice;

class JsonClient : public QObject
{
    Q_OBJECT
public:
    explicit JsonClient(QIODevice * device, QObject *parent = 0);

    ~JsonClient();
    
    QtJson::JsonObject createError(const QString & name, const QString & description);

signals:
    
private slots:
    void onMessageReceived();
private:
    Protocole * m_protocole;
};

#endif // JSONCLIENT_H
