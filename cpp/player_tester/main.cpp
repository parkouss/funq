#include "fenPrincipale.h"
#include "scleHook.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    ScleHooq::activation();
    
    fenPrincipale w;
    w.show();
    
    return a.exec();
}
