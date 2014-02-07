#include "sclehooqattacher.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

ScleHooqAttacher::ScleHooqAttacher(const QString & exe, const QStringList & args, QObject *parent) :
    QObject(parent), m_exe(exe), m_args(args)
{
}

void ScleHooqAttacher::start() {
    m_process.start(m_exe, m_args);
    if (m_process.waitForStarted()) {
        QDir appPath(QCoreApplication::applicationDirPath());
        QString libraryPath = appPath.absoluteFilePath("scleHooq.dll");
        qDebug() << libraryPath;
        wchar_t path[_MAX_PATH];
        const int pathLength = libraryPath.toWCharArray(path);
        path[pathLength] = 0;
        HMODULE library = ::LoadLibraryW(path);
        qDebug() << library;
        FARPROC hook = ::GetProcAddress(library, "_Z11installHooqP11HINSTANCE__m");
        qDebug() << "hooq" << hook;
        typedef void(*Installer)(HINSTANCE, DWORD);
        Installer installer = reinterpret_cast<Installer>(hook);
        (*installer)(library, m_process.pid()->dwThreadId);
    }
}
