#ifndef SCLE_HOOQ
#define SCLE_HOOQ

#include <QObject>

class QTcpServer;

class ScleHooq : public QObject {
	Q_OBJECT
	
public:
    static void activation(bool check_env=true);
	
    enum MODE {
        PLAYER,
        PICK
    };

	~ScleHooq();
	
protected:
    ScleHooq(MODE mode);

	bool listen(quint16 port);
	
	inline QTcpServer * server() const { return m_server; }
	
protected slots:
	void acceptConnection();
	
private:
    static bool registerPick();
    static bool unRegisterPick();
    static bool hook(void** data);
    static void active_hook_player(MODE mode);
    bool eventFilter(QObject* receiver, QEvent* event);

	static ScleHooq * _instance;
	
	QTcpServer * m_server;
    MODE m_mode;
};

#endif // SCLE_HOOQ
