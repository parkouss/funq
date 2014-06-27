from PySide import QtGui, QtCore
import sys

def exec_dlg(self, dlgclass):
    def wrapper():
        dlg = dlgclass(self)
        dlg.exec_()
        # really destroy the dialog
        dlg.deleteLater()
    return wrapper

class MainWindow(QtGui.QMainWindow):
    def __init__(self, dialogs):
        QtGui.QMainWindow.__init__(self)
        self.setObjectName("mainWindow")
        self.setCentralWidget(QtGui.QWidget())
        layout = QtGui.QVBoxLayout()
        statusbar = QtGui.QStatusBar()
        statusbar.setObjectName("statusBar")
        self.statuslabel = QtGui.QLabel()
        statusbar.insertPermanentWidget(0, self.statuslabel)
        self.setStatusBar(statusbar)
        self.centralWidget().setLayout(layout)
        for name, dlgclass in dialogs.iteritems():
            btn = QtGui.QPushButton(name)
            btn.setObjectName(name)
            btn.clicked.connect(exec_dlg(self, dlgclass))
            layout.addWidget(btn)

class SimpleDialog(QtGui.QDialog):
    def __init__(self, parent):
        QtGui.QDialog.__init__(self, parent)
        self.setLayout(QtGui.QVBoxLayout())
        for w in self._create_widgets():
            self.layout().addWidget(w)
    
    def showResult(self, msg):
        self.parent().statuslabel.setText(msg)
    
    def _create_widgets(self):
        raise NotImplementedError

class ClickDialog(SimpleDialog):
    def _create_widgets(self):
        btn = QtGui.QPushButton("click me")
        btn.clicked.connect(lambda: self.showResult("clicked !"))
        yield btn

class DoubleClickDialog(SimpleDialog):
    def _create_widgets(self):
        w = QtGui.QWidget()
        w.resize(100, 100)
        yield w
    
    def mouseDoubleClickEvent (self, e):
        self.showResult("double clicked !")
        

def main():
    dialogs = {
        "click": ClickDialog,
        'doubleclick': DoubleClickDialog
    }
    
    app = QtGui.QApplication(sys.argv)
    win = MainWindow(dialogs)
    win.resize(800, 600)
    win.show()
    app.exec_()

if __name__ == '__main__':
    main()
