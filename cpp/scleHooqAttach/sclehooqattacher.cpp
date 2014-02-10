#include <QtGlobal>
#include "sclehooqattacher.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

#ifdef Q_WS_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

ScleHooqAttacher::ScleHooqAttacher(const QString & exe, const QStringList & args, QObject *parent) :
    QObject(parent), m_exe(exe), m_args(args)
{
    m_process.setProcessChannelMode(QProcess::ForwardedChannels);
    m_process.setWorkingDirectory(QFileInfo(exe).absolutePath());

    QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
    #ifndef Q_WS_WIN
    QString ld_preload = env.value("LD_PRELOAD");
    if (! ld_preload.isEmpty()) {
        ld_preload += " ";
    }
    QDir appPath(QCoreApplication::applicationDirPath());
    ld_preload += appPath.absoluteFilePath("libscleHooq.so");
    env.insert("LD_PRELOAD", ld_preload);
    #endif
    m_process.setProcessEnvironment(env);
}

void ScleHooqAttacher::start() {
    m_process.start(m_exe, m_args);
    #ifdef Q_WS_WIN
    if (m_process.waitForStarted()) {
        QDir appPath(QCoreApplication::applicationDirPath());
        QString libraryPath = appPath.absoluteFilePath("scleHooq.dll");
        wchar_t path[_MAX_PATH];
        const int pathLength = libraryPath.toWCharArray(path);
        path[pathLength] = 0;
        HMODULE library = ::LoadLibraryW(path);
        if (!library) {
            qDebug() << QString("Impossible de charger la librairie %1").arg(libraryPath);
            m_process.kill();
            return;
        }
        FARPROC hook = ::GetProcAddress(library, "_Z11installHooqP11HINSTANCE__m");
        if (!hook) {
            qDebug() << "Impossible de trouver la fonction installHooq";
            m_process.kill();
            return;
        }
        typedef void(*Installer)(HINSTANCE, DWORD);
        Installer installer = reinterpret_cast<Installer>(hook);
        (*installer)(library, m_process.pid()->dwThreadId);
    }
#endif
}