/*
Copyright: SCLE SFE
Contributors: https://github.com/parkouss/funq/graphs/contributors

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

#include "widgets.h"

#include <QApplication>

template <typename T>
inline void execDialog(QLabel * statusLabel, QWidget * parent) {
    T dialog(statusLabel, parent);
    dialog.exec();
}

int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

    if (app.arguments().contains("--show-message-box-at-startup")) {
        // This is needed to test if the injection of libFunq also works if Qt's
        // main event loop is not called directly at application startup. A
        // message box blocks the whole application, so by closing this dialog
        // with funq we can see if funq was successfully injected into the
        // blocking application.
        QMessageBox::information(0, "funq", "click me away");
    }

    MainWindow win;
    win.addDialogButton("action", &execDialog<ActionDialog>);
    win.addDialogButton("click", &execDialog<ClickDialog>);
    win.addDialogButton("widgetclick", &execDialog<WidgetClickDialog>);
    win.addDialogButton("doubleclick", &execDialog<DoubleClickDialog>);
    win.addDialogButton("keyclick", &execDialog<KeyClickDialog>);
    win.addDialogButton("retrieve", &execDialog<RetrieveWidget>);
    win.addDialogButton("shortcut", &execDialog<ShortcutDialog>);
    win.addDialogButton("table", &execDialog<TableDialog>);
    win.addDialogButton("combobox", &execDialog<ComboBoxDialog>);
    win.resize(800, 600);
    win.show();

    return app.exec();
}
