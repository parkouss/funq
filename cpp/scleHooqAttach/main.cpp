#include <QtCore/QCoreApplication>
#include "sclehooqattacher.h"
#include <QTimer>
#include <QFileInfo>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    int argc_app_pos = 1;
    int port = 0;
    while (argc_app_pos < argc) {
        const char * arg = argv[argc_app_pos];
        if (strcmp(arg, "--port") == 0) {
            if (argc_app_pos+1 >= argc) {
                qDebug() << "Argument port requis";
                return 1;
            }
            port = QString(argv[argc_app_pos+1]).toInt();
            argc_app_pos += 2;
        } else if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            printf("USAGE: %s [--port PORT] EXECUTABLE [ARG1] ... [ARGN]\n", argv[0]);
            return 0;
        } else {
            break;
        }
    }

    if (argc_app_pos >= argc) {
        qDebug() << "Nom de l'executable a hooquer requis en argument";
        return 1;
    }


    QString exePath = argv[argc_app_pos];
    int app_arg_pos = argc_app_pos+1;
    QStringList appArgs;
    while (app_arg_pos < argc) {
        appArgs << argv[app_arg_pos];
        app_arg_pos++;
    }

    ScleHooqAttacher attacher(exePath, appArgs, port);

    QObject::connect(&attacher.process(), SIGNAL(finished(int)), &a, SLOT(quit()));
    QTimer::singleShot(0, &attacher, SLOT(start()));

    a.exec();
    return attacher.process().exitCode();
}
