#include "protocole.h"
#include <QDebug>

Protocole::Protocole(QObject *parent) :
    QObject(parent), m_device(0), m_messageSize(0)
{
}


void Protocole::setDevice(QIODevice *device) {
    if (m_device) {
        disconnect(m_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        disconnect(m_device, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));
    }
    if (device) {
        connect(device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(device, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));
    }
    m_device = device;
}

void Protocole::onReadyRead() {
    while (1) {
        if (m_messageSize == 0) {
            // attente d'entete
            if (!m_device->canReadLine()) {
                return; // we need more data
            }
            QString entete =  QString(m_device->readLine());
            bool ok = false;
            m_messageSize = entete.toLongLong(&ok);
            if (!ok || m_messageSize == 0) {
                qDebug() << QString("Erreur de réception d'entete: %1").arg(entete);
                close();
                return;
            }
        }
        if (m_device->bytesAvailable() < m_messageSize) {
            return; // we need more data
        }

        // the message now
        m_receivedMessages.push_front(m_device->read(m_messageSize));
        m_messageSize = 0;
        emit messageReceived();
    }
}

QByteArray Protocole::nextAvailableMessage() {
    if (m_receivedMessages.isEmpty()) {
        return QByteArray();
    }
    QByteArray message = m_receivedMessages.front();
    m_receivedMessages.removeFirst();
    return message;
}

void Protocole::onBytesWritten(qint64 written) {
    m_messagesToSend = m_messagesToSend.mid(written);
    if (! m_messagesToSend.isEmpty()) {
        m_device->write(m_messagesToSend);
    }
}

bool Protocole::sendMessage(const QByteArray &ba) {
    if (!m_device) {
        return false;
    }
    bool toSend = m_messagesToSend.isEmpty();
    m_messagesToSend.append(QString::number(ba.size()));
    m_messagesToSend.append('\n');
    m_messagesToSend.append(ba);
    if (toSend) {
        m_device->write(m_messagesToSend);
    }
    return true;
}

void Protocole::close() {
    if (m_device) {
        m_device->close();
    }
}
