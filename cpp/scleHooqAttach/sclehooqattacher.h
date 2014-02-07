#ifndef SCLEHOOQATTACHER_H
#define SCLEHOOQATTACHER_H

#include <QObject>
#include <QProcess>

class ScleHooqAttacher : public QObject
{
    Q_OBJECT
public:
    explicit ScleHooqAttacher(const QString & exe, const QStringList & args=QStringList(), QObject *parent = 0);
    
    QProcess & process() { return m_process; }

signals:
    
public slots:
    void start();
private:
    QString m_exe;
    QStringList m_args;
    QProcess m_process;
};

#endif // SCLEHOOQATTACHER_H
