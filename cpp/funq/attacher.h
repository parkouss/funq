#ifndef SCLEHOOQATTACHER_H
#define SCLEHOOQATTACHER_H

#include <QtGlobal>
#include <QObject>
#include <QProcess>

class QSocketNotifier;

class Attacher : public QObject
{
    Q_OBJECT
public:
    explicit Attacher(const QString & exe, const QStringList & args=QStringList(), int port=0, bool pickMode=false, QObject *parent = 0);
    
    QProcess & process() { return m_process; }

    int returnCode();
#ifndef Q_WS_WIN
    static void termSignalHandler(int unused);
#endif
signals:
    
public slots:
    void start();
#ifndef Q_WS_WIN
    void handleSigTerm();
#endif
private:
    QString m_exe;
    QStringList m_args;
    QProcess m_process;
    bool m_wellStarted;
#ifndef Q_WS_WIN
    static int m_sigtermFd[2];
    QSocketNotifier *m_snTerm;
#endif
};

#endif // SCLEHOOQATTACHER_H
