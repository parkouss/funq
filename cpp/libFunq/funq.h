#ifndef FUNQ_H
#define FUNQ_H

#include <QObject>

class QTcpServer;
class Pick;

class Funq : public QObject
{
    Q_OBJECT
public:
    static void activate(bool check_activation=false);

    enum MODE {
         PLAYER,
         PICK
    };

protected:
    explicit Funq(MODE mode);
    bool eventFilter(QObject* receiver, QEvent* event);
    
signals:
    
private slots:
    void onNewConnection();

private:
    static bool registerPick();
    static bool unRegisterPick();
    static bool hook(void** data);
    static void active_hook_player(MODE mode);

    static Funq * _instance;

    MODE m_mode;
    QTcpServer * m_server;
    
    Pick * m_pick;
};

#endif // FUNQ_H
