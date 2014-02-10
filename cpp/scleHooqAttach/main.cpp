#include <QtCore/QCoreApplication>
#include "sclehooqattacher.h"
#include <QTimer>
#include <QFileInfo>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    if (argc < 2) {
        qDebug() << "Nom de l'executable a hooquer requis en argument";
        return 1;
    }

    QString exePath = argv[1];

    ScleHooqAttacher attacher(exePath);

    QObject::connect(&attacher.process(), SIGNAL(finished(int)), &a, SLOT(quit()));
    QTimer::singleShot(0, &attacher, SLOT(start()));

    a.exec();
    return attacher.process().exitCode();
}
