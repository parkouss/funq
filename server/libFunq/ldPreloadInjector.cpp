/*
Copyright: SCLE SFE
Contributor: Julien Pagès <j.parkouss@gmail.com>

This software is a computer program whose purpose is to test graphical
applications written with the QT framework (http://qt.digia.com/).

This software is governed by the CeCILL v2.1 license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL v2.1 license and that you accept its terms.
*/

#include "funq.h"

#include <QCoreApplication>

#include <iostream>
#include <pthread.h>
#include <unistd.h>

static void * injectorThread(void *) {
    // Wait until QApplication instance exists! Many Qt features must not be
    // used before QApplication is initialized (could lead to strange and fatal
    // errors), so we don't use Qt at all until QCoreApplication::startingUp()
    // returns true.
    //
    // This must be done in a dedicated thread, otherwise the main thread is
    // blocked and QApplication gets never initialized.
    while (QCoreApplication::startingUp()) {
        usleep(50000);  // wait 50ms
    }
    Funq::activate();
    return NULL;
}

class Injector {
    pthread_t m_thread;

public:
    Injector() {
        if (pthread_create(&m_thread, NULL, injectorThread, NULL)) {
            std::cerr << "Failed to create thread!" << std::endl;
        }
    }
};

// The constructor of this static object will be called at application startup,
// so we can use the constructor to inject our Funq module (TCP server etc.).
static Injector injector;
