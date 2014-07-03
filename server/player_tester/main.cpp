#include "fenPrincipale.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    fenPrincipale w;
    w.show();
    
    return a.exec();
}
