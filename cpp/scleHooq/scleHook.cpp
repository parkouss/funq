#include "scleHook.h"
#include "Communication.h"
#include "Player.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <QCoreApplication>
#include <cstdlib>

using namespace std;
using namespace Hooq;

#define DEFAUT_HOOQ_PORT 9999;

/*static*/
ScleHooq * ScleHooq::_instance = 0;

ScleHooq::ScleHooq() : QObject(), m_server(new QTcpServer) {
	Q_ASSERT(!_instance);
	_instance = this;
	
	connect(m_server,
	        SIGNAL(newConnection()),
            this,
            SLOT(acceptConnection()));
}

ScleHooq::~ScleHooq() { delete m_server; }

bool ScleHooq::listen(quint16 port) {
	return m_server->listen(QHostAddress::LocalHost, port);
}

void ScleHooq::acceptConnection() {
	Player::instance(m_server->nextPendingConnection());
}

void active_hook_player() {
	Q_ASSERT(QCoreApplication::instance());
	
	int port = DEFAUT_HOOQ_PORT;
	const char * env_port = getenv("SCLEHOOQ_PORT");
	if (env_port) {
		int _port = atoi(env_port);
		if (_port > 0) {
			port = _port;
		}
	}
	
	ScleHooq * hook = new ScleHooq;
	if (!hook->listen(port)) {
		qDebug() << "Impossible d'initialiser scleHooq. Erreur:\n\t"
				 << hook->server()->errorString();
	} else {
		qDebug() << "scleHooq est initialisé sur le port " << port << ".";
	}
	
	QObject::connect(
		QCoreApplication::instance(),
		SIGNAL(aboutToQuit()),
		hook,
		SLOT(deleteLater()));
}

/* static */
void ScleHooq::activation() {
	const char * env_activation = getenv("SCLEHOOQ_ACTIVATION");
	bool active = env_activation && strcmp(env_activation, "1") == 0;
	if (active) {
		active_hook_player();
	}
}
