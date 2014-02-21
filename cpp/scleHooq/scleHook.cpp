#include "scleHook.h"
#include "ObjectHookName.h"
#include "Player.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <QCoreApplication>
#include <QInternal>
#include <cstdlib>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMouseEvent>

using namespace std;
using namespace Hooq;

extern Q_GUI_EXPORT bool qt_use_native_dialogs;

#define DEFAUT_HOOQ_PORT 9999;

/*static*/
ScleHooq * ScleHooq::_instance = 0;

ScleHooq::ScleHooq(ScleHooq::MODE mode) : QObject(), m_server(0), m_mode(mode) {
	Q_ASSERT(!_instance);
	_instance = this;
	
    if (mode == ScleHooq::PLAYER) {
        m_server = new QTcpServer;
        connect(m_server,
	        SIGNAL(newConnection()),
            this,
            SLOT(acceptConnection()));
    }
}

ScleHooq::~ScleHooq() {
    if (m_mode == ScleHooq::PLAYER) {
        delete m_server;
    } else {
        unRegisterPick();
    }
}

bool ScleHooq::listen(quint16 port) {
    if (m_mode == ScleHooq::PLAYER) {
        return m_server->listen(QHostAddress::LocalHost, port);
    }
    return false;
}

void ScleHooq::acceptConnection() {
    QIODevice * socket = m_server->nextPendingConnection();
    Player * player = new Player(socket);
    connect(socket, SIGNAL(destroyed()), player, SLOT(deleteLater()));
}

void ScleHooq::active_hook_player(ScleHooq::MODE mode) {
	Q_ASSERT(QCoreApplication::instance());
	qt_use_native_dialogs = false;
	int port = DEFAUT_HOOQ_PORT;
	const char * env_port = getenv("SCLEHOOQ_PORT");
	if (env_port) {
		int _port = atoi(env_port);
		if (_port > 0) {
			port = _port;
		}
	}
	
    ScleHooq * hook = new ScleHooq(mode);
    if (mode == ScleHooq::PLAYER) {
        if (!hook->listen(port)) {
            qDebug() << "Impossible d'initialiser scleHooq. Erreur:\n\t"
                     << hook->server()->errorString();
        } else {
            qDebug() << "scleHooq est initialisé sur le port " << port << ".";
        }
    } else {
        if (registerPick()) {
            qDebug() << "Hooq mode PICK en action !";
        } else {
            qDebug() << "Erreur interne, impossible d'utiliser le mode PICK de Hooq";
        }
    }
	
	QObject::connect(
		QCoreApplication::instance(),
		SIGNAL(aboutToQuit()),
		hook,
		SLOT(deleteLater()));
}

/* static */
void ScleHooq::activation(bool check_env) {
    bool active = true;
    if (check_env) {
        const char * env_activation = getenv("SCLEHOOQ_ACTIVATION");
        active = env_activation && strcmp(env_activation, "1") == 0;
    }
    if (active) {
        const char * env_pick = getenv("SCLEHOOQ_MODE_PICK");
        ScleHooq::MODE mode = (env_pick && strcmp(env_pick, "1") == 0) ?
                    ScleHooq::PICK : ScleHooq::PLAYER;
        active_hook_player(mode);
	}
}

/* static */
bool ScleHooq::registerPick() {
    return QInternal::registerCallback(QInternal::EventNotifyCallback, hook);
}

/* static */
bool ScleHooq::unRegisterPick() {
    return QInternal::unregisterCallback(QInternal::EventNotifyCallback, hook);
}

/* static */
bool ScleHooq::hook(void** data)
{
    QObject* receiver = reinterpret_cast<QObject*>(data[0]);
    QEvent* event = reinterpret_cast<QEvent*>(data[1]);
    return _instance->eventFilter(receiver, event);
}

bool ScleHooq::eventFilter(QObject* receiver, QEvent* event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *evt = static_cast<QMouseEvent *>(event);
        if (1/*evt->modifiers() & Qt::ShiftModifier && evt->modifiers() &  Qt::ControlModifier*/) {
            QString path = QString("WIDGET: `%1` (pos: %2, %3)")
                    .arg(ObjectHookName::objectPath(receiver))
                    .arg(evt->pos().x())
                    .arg(evt->pos().y());
            printf("%s\n", path.toStdString().c_str());
            for(int i = 0; i < receiver->metaObject()->propertyCount(); ++i)
            {
                QMetaProperty property = receiver->metaObject()->property(i);
                QString strValue = property.read(receiver).toString();
                if (! strValue.isEmpty()) {
                    printf("\t%s: %s\n",
                        property.name(), strValue.toStdString().c_str());
                }
            }
        }
    }
    return false;
}
