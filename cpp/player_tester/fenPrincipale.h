#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class fenPrincipale;
}

class fenPrincipale : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit fenPrincipale(QWidget *parent = 0);
    ~fenPrincipale();
    
private slots:
    void btnClicked();
private:
    Ui::fenPrincipale *ui;
};

#endif // MAINWINDOW_H
