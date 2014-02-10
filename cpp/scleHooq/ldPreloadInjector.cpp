
#include <QCoreApplication>
#include <QDebug>
#include "scleHook.h"
#include <dlfcn.h>

int QCoreApplication::exec() {

    typedef int QCoreApplicationExec();

    QCoreApplicationExec * originalExec =
            (QCoreApplicationExec *) dlsym(RTLD_NEXT, "_ZN16QCoreApplication4execEv");
    if (!originalExec) {
        qDebug() << "impossible de localiser la fonction originale QCoreApplication::exec(). Abandon";
        return 1;
    }
    ScleHooq::activation(false);
    return originalExec();
}
