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

#include <QtCore>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

class SimpleDialog : public QDialog {
    Q_OBJECT
    QLabel * _statusLabel;

public:
    SimpleDialog(QLabel * statusLabel, QWidget * parent)
        : QDialog(parent), _statusLabel(statusLabel) {
        setLayout(new QVBoxLayout());
    }

    virtual ~SimpleDialog() {}

protected slots:
    void showResult(const QString & msg) { _statusLabel->setText(msg); }
};

class ActionDialog : public SimpleDialog {
    Q_OBJECT
public:
    ActionDialog(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        QMenuBar * bar = new QMenuBar(this);
        QMenu * menu = bar->addMenu("menu");

        QAction * nonblockingAction = new QAction("nonblocking", this);
        nonblockingAction->setObjectName("nonblockingAction");
        connect(nonblockingAction, SIGNAL(triggered()), this,
                SLOT(nonblockingActionTriggered()));
        addAction(nonblockingAction);
        menu->addAction(nonblockingAction);

        QAction * blockingAction = new QAction("blocking", this);
        blockingAction->setObjectName("blockingAction");
        connect(blockingAction, SIGNAL(triggered()), this,
                SLOT(blockingActionTriggered()));
        addAction(blockingAction);
        menu->addAction(blockingAction);
    }

private slots:
    void nonblockingActionTriggered() { showResult("nonblocking triggered !"); }

    void blockingActionTriggered() {
        showResult("blocking triggered !");
        QMessageBox::information(0, "funq", "click me away");  // blocking!
    }
};

class ClickDialog : public SimpleDialog {
    Q_OBJECT
public:
    ClickDialog(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        QPushButton * btn = new QPushButton("click me");
        connect(btn, SIGNAL(clicked()), this, SLOT(btnClicked()));
        layout()->addWidget(btn);
    }

private slots:
    void btnClicked() { showResult("clicked !"); }
};

class WidgetClickDialog : public SimpleDialog {
    Q_OBJECT
public:
    WidgetClickDialog(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        QWidget * w = new QWidget();
        w->resize(100, 100);
        layout()->addWidget(w);
    }

    void mouseReleaseEvent(QMouseEvent * e) {
        if (e->button() == Qt::LeftButton) {
            showResult("left clicked !");
        } else if (e->button() == Qt::RightButton) {
            showResult("right clicked !");
        } else if (e->button() == Qt::MiddleButton) {
            showResult("middle clicked !");
        } else {
            showResult("clicked with unhandled mouse button :(");
        }
    }
};

class DoubleClickDialog : public SimpleDialog {
    Q_OBJECT
public:
    DoubleClickDialog(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        QWidget * w = new QWidget();
        w->resize(100, 100);
        layout()->addWidget(w);
    }

    void mouseDoubleClickEvent(QMouseEvent * e) {
        Q_UNUSED(e);
        showResult("double clicked !");
    }
};

class KeyClickDialog : public SimpleDialog {
    Q_OBJECT
public:
    KeyClickDialog(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        QLineEdit * edt = new QLineEdit();
        connect(edt, SIGNAL(textEdited(QString)), this,
                SLOT(showResult(QString)));
        layout()->addWidget(edt);
    }
};

class RetrieveWidget : public SimpleDialog {
    Q_OBJECT
public:
    RetrieveWidget(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        layout()->addWidget(new QLabel("hello"));
        layout()->addWidget(new QComboBox());
    }
};

class ShortcutDialog : public SimpleDialog {
    Q_OBJECT
public:
    ShortcutDialog(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        QStringList sequences;
        sequences << "F2"
                  << "DOWN"
                  << "ENTER"
                  << "CTRL+C";
        foreach (const QString & sequence, sequences) {
            QShortcut * shortcut = new QShortcut(QKeySequence(sequence), this);
            connect(shortcut, SIGNAL(activated()), this,
                    SLOT(shortcutActivated()));
        }
    }

private slots:
    void shortcutActivated() {
        QString sequence =
            qobject_cast<QShortcut *>(sender())->key().toString();
        showResult("Shortcut: " + sequence.toUpper());
    }
};

class TableDialog : public SimpleDialog {
    Q_OBJECT
public:
    TableDialog(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        QTableWidget * table = new QTableWidget(2, 3);
        table->setHorizontalHeaderLabels(QStringList() << "C1"
                                                       << "C2"
                                                       << "C3");
        table->setVerticalHeaderLabels(QStringList() << "R1"
                                                     << "R2");
        for (int col = 0; col < 3; ++col) {
            for (int row = 0; row < 2; ++row) {
                QString text = QString("%1.%2").arg(col).arg(row);
                table->setItem(row, col, new QTableWidgetItem(text));
            }
        }
        table->horizontalHeader()->setObjectName("H");
        connect(table->horizontalHeader(), SIGNAL(sectionClicked(int)), this,
                SLOT(hheaderClicked(int)));
        table->verticalHeader()->setObjectName("V");
        connect(table->verticalHeader(), SIGNAL(sectionClicked(int)), this,
                SLOT(vheaderClicked(int)));
        layout()->addWidget(table);
    }

private slots:
    void hheaderClicked(int logicalIndex) {
        showResult("H Header clicked: " + QString::number(logicalIndex));
    }

    void vheaderClicked(int logicalIndex) {
        showResult("V Header clicked: " + QString::number(logicalIndex));
    }
};

class ComboBoxDialog : public SimpleDialog {
    Q_OBJECT
public:
    ComboBoxDialog(QLabel * statusLabel, QWidget * parent)
        : SimpleDialog(statusLabel, parent) {
        QComboBox * combobox = new QComboBox();
        for (int i = 0; i < 10; ++i) {
            combobox->addItem(QString("Item %1").arg(i));
        }
        connect(combobox, SIGNAL(currentIndexChanged(QString)), this,
                SLOT(currentIndexChanged(QString)));
        layout()->addWidget(combobox);
    }

private slots:
    void currentIndexChanged(const QString & text) {
        showResult("Text: " + text);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
    QLabel * _statusLabel;

public:
    typedef void (*ButtonHandler)(QLabel *, QWidget *);

    MainWindow() {
        setObjectName("mainWindow");
        setCentralWidget(new QWidget());
        centralWidget()->setLayout(new QVBoxLayout());
        QStatusBar * statusbar = new QStatusBar();
        statusbar->setObjectName("statusBar");
        statusbar->insertPermanentWidget(0, _statusLabel = new QLabel());
        setStatusBar(statusbar);
    }

    void addDialogButton(const QString & name, ButtonHandler handler) {
        QPushButton * btn = new QPushButton(name);
        btn->setObjectName(name);
        btn->setProperty("handler", QVariant::fromValue(handler));
        connect(btn, SIGNAL(clicked()), this, SLOT(btnClicked()));
        centralWidget()->layout()->addWidget(btn);
    }

private slots:
    void btnClicked() {
        ButtonHandler handler =
            sender()->property("handler").value<ButtonHandler>();
        if (handler) (*handler)(_statusLabel, this);
    }
};

Q_DECLARE_METATYPE(MainWindow::ButtonHandler);
