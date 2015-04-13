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

#include "protocole.h"
#include <QDebug>

Protocole::Protocole(QObject *parent) :
    QObject(parent), m_device(0), m_messageSize(0)
{
}


void Protocole::setDevice(QIODevice *device) {
    if (m_device) {
        disconnect(m_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    }
    if (device) {
        connect(device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    }
    m_device = device;
}

void Protocole::onReadyRead() {
    while (1) {
        if (m_messageSize == 0) {
            // waiting header
            if (!m_device->canReadLine()) {
                return; // we need more data
            }
            QString entete =  QString(m_device->readLine());
            bool ok = false;
            m_messageSize = entete.toLongLong(&ok);
            if (!ok || m_messageSize == 0) {
                qDebug() << QString("Error while reading frame header: %1").arg(entete);
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

bool Protocole::sendMessage(const QByteArray &ba) {
    if (!m_device) {
        return false;
    }
    QByteArray messageToSend;
    messageToSend.append(QString::number(ba.size()));
    messageToSend.append('\n');
    messageToSend.append(ba);
    m_device->write(messageToSend);
    return true;
}

void Protocole::close() {
    if (m_device) {
        m_device->close();
    }
}
