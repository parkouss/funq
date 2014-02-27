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

    void close();
signals:
    void messageReceived();
    
private slots:
    void onReadyRead();
    void onBytesWritten(qint64);
private:
    QIODevice * m_device;
    qlonglong m_messageSize;
    QList<QByteArray> m_receivedMessages;
    QByteArray m_messagesToSend;
};

#endif // PROTOCOLE_H
