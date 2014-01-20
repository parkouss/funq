/*
	Hooq: Qt4 UI recording, playback, and testing toolkit.
	Copyright (C) 2009  Mendeley Limited <copyright@mendeley.com>
	Copyright (C) 2009  Frederick Emmott <mail@fredemmott.co.uk>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "Marshall.h"

#include "Logger.h"
#include "Player.h"

#include "../common/Communication.h"

#include <QCoreApplication>
#include <QHostAddress>
#include <QTcpSocket>

#include <QFile>
#include <QTextStream>

namespace Hooq
{


Marshall* Marshall::m_instance = 0;

Marshall* Marshall::instance()
{
	if(!m_instance)
	{
		m_instance = new Marshall();
    }
	return m_instance;
}

QTcpSocket* Marshall::m_socket;

Marshall::Marshall()
: QObject()
{
	m_socket = new QTcpSocket();
    m_socket->connectToHost(QHostAddress::LocalHost, HOOQ_PORT);
    m_socket->waitForConnected(10000);
//    Q_ASSERT(m_socket->state() == QTcpSocket::ConnectedState && m_socket->isReadable());
	connect(
		m_socket,
		SIGNAL(readyRead()),
		SLOT(readCommand())
    );

    qDebug() << "init Marshall";

    m_socket->write("INIT_COM\n");
    m_socket->flush();

	qAddPostRoutine(flushSocket);
}

void Marshall::flushSocket()
{
	m_socket->flush();
	m_socket->waitForBytesWritten(1000);
	m_socket->close();
	delete instance();
	delete m_socket;
}

Marshall::~Marshall()
{
}

void Marshall::reconnect()
{
    disconnect(m_socket, 0, 0, 0);
	connect(
		m_socket,
		SIGNAL(readyRead()),
		SLOT(readCommand())
	);
	readCommand();
}

void Marshall::readCommand()
{
    while(m_socket->canReadLine())
	{
		const QByteArray command = m_socket->readLine();

        qDebug() << "Command received: " << command;

		if(command == "RECORD\n")
		{
			Logger::instance(m_socket);
			break;
		}
		else if(command == "PLAY\n")
		{
			Player* player = Player::instance(m_socket);
			connect(
				player,
				SIGNAL(finished()),
				SLOT(reconnect())
			);
			break;
		}
		else if(command == "DIE\n")
		{
			m_socket->write("ACK\n");
			QCoreApplication::quit();
			break;
		}
        else if(command == "TEST_COM\n")
        {
            m_socket->write("ACK_COM\n");
        }
		else
		{
            qFatal("Unknown command: %s", command.constData());
		}
	}
}

} // namespace
