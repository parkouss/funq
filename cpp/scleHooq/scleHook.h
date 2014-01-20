#ifndef SCLE_HOOQ
#define SCLE_HOOQ

#include <QObject>

class QTcpServer;

class ScleHooq : public QObject {
	Q_OBJECT
	
	public:
	static void activation();
	
	ScleHooq();
	~ScleHooq();
	
	bool listen(quint16 port);
	
	inline QTcpServer * server() const { return m_server; }
	
	protected slots:
	void acceptConnection();
	
	private:
	static ScleHooq * _instance;
	
	QTcpServer * m_server;
};

#endif // SCLE_HOOQ
