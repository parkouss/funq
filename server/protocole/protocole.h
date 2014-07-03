#ifndef PROTOCOLE_H
#define PROTOCOLE_H

#include <QObject>
#include <QByteArray>
#include <QIODevice>
#include <QList>

class Protocole : public QObject
{
    Q_OBJECT
public:
    explicit Protocole(QObject *parent = 0);
    
    void setDevice(QIODevice * device);

    bool sendMessage(const QByteArray & ba);

    QByteArray nextAvailableMessage();
    
    inline bool hasAvailableMessage() { return ! m_receivedMessages.isEmpty(); }

    void close();
signals:
    void messageReceived();
    
private slots:
    void onReadyRead();
private:
    QIODevice * m_device;
    qlonglong m_messageSize;
    QList<QByteArray> m_receivedMessages;
};

#endif // PROTOCOLE_H
