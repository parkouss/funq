#include "funq.h"
#include "player.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QEvent>
#include <QCoreApplication>
#include <QMetaProperty>
#include <QMouseEvent>
#include "objectpath.h"

#define DEFAUT_HOOQ_PORT 9999;

#ifdef Q_WS_WIN
extern Q_GUI_EXPORT bool qt_use_native_dialogs;
#endif

/*static*/
Funq * Funq::_instance = 0;

Funq::Funq(Funq::MODE mode) :
    QObject(), m_mode(mode), m_server(0)
{
    Q_ASSERT(!_instance);
    _instance = this;

    if (mode == Funq::PLAYER) {
        m_server = new QTcpServer(this);
        connect(m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    }
}

void Funq::onNewConnection() {
    QTcpSocket * socket = m_server->nextPendingConnection();
    Player * player = new Player(socket, this);

    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(destroyed()), player, SLOT(deleteLater()));
}

void Funq::active_hook_player(Funq::MODE mode) {
    Q_ASSERT(QCoreApplication::instance());
#ifdef Q_WS_WIN
    qt_use_native_dialogs = false;
#endif

    int port = DEFAUT_HOOQ_PORT;
    const char * env_port = getenv("FUNQ_PORT");
    if (env_port) {
        int _port = atoi(env_port);
        if (_port > 0) {
            port = _port;
        }
    }

    Funq * hook = new Funq(mode);
    if (mode == Funq::PLAYER) {
        if (!hook->m_server->listen(QHostAddress::LocalHost, port)) {
            qDebug() << "Impossible d'initialiser Funq. Erreur:\n\t"
                     << hook->m_server->errorString();
        } else {
            qDebug() << "Funq est initialisé sur le port " << port << ".";
        }
    } else {
        if (registerPick()) {
            qDebug() << "Funq mode PICK en action !";
        } else {
            qDebug() << "Erreur interne, impossible d'utiliser le mode PICK de Funq";
        }
    }

    QObject::connect(
        QCoreApplication::instance(),
        SIGNAL(aboutToQuit()),
        hook,
        SLOT(deleteLater()));
}

bool Funq::registerPick() {
    return QInternal::registerCallback(QInternal::EventNotifyCallback, hook);
}

/* static */
bool Funq::unRegisterPick() {
    return QInternal::unregisterCallback(QInternal::EventNotifyCallback, hook);
}

/* static */
bool Funq::hook(void** data)
{
    QObject* receiver = reinterpret_cast<QObject*>(data[0]);
    QEvent* event = reinterpret_cast<QEvent*>(data[1]);
    return _instance->eventFilter(receiver, event);
}

bool Funq::eventFilter(QObject* receiver, QEvent* event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *evt = static_cast<QMouseEvent *>(event);
        if (evt->modifiers() & Qt::ShiftModifier && evt->modifiers() &  Qt::ControlModifier) {
            QString path = QString("WIDGET: `%1` (pos: %2, %3)")
                    .arg(objectPath(receiver))
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

void Funq::activate(bool check_activation) {
    if (check_activation) {
        const char * env_activation = getenv("FUNQ_ACTIVATION");
        if (! (env_activation && strcmp(env_activation, "1") == 0)) {
            return;
        }
    }
    const char * env_pick = getenv("FUNQ_MODE_PICK");
    Funq::MODE mode = (env_pick && strcmp(env_pick, "1") == 0) ? Funq::PICK : Funq::PLAYER;
    active_hook_player(mode);
}
