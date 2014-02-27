#include "fenPrincipale.h"
#include "ui_fenPrincipale.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include <QDebug>

fenPrincipale::fenPrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::fenPrincipale)
{
    ui->setupUi(this);

    connect(
        ui->btnTest,
        SIGNAL(clicked()),
        SLOT(btnClicked())
    );

    QStandardItemModel * model = new QStandardItemModel(4, 4, this);
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            model->setItem(row, column, item);
        }
    }

    QStandardItemModel * model_tree = new QStandardItemModel(this);
    for (int i = 0; i < 3; ++i) {
        QStandardItem *item = new QStandardItem(QString("item %1").arg(i));
        model_tree->invisibleRootItem()->appendRow(item);

        for (int j = 0; j < 3; ++j) {
            QStandardItem *item_bis = new QStandardItem(QString("item %1-%2").arg(i).arg(j));
            item->appendRow(item_bis);
        }
    }

    ui->listView->setModel(model);
    ui->tableView->setModel(model);
    ui->treeView->setModel(model_tree);
    ui->comboBox->setModel(model);
}

void fenPrincipale::btnClicked() {
    qDebug() << "Bouton clicked";
    QMessageBox::information(this, "Bouton clicked", "Bouton clicked");
}

fenPrincipale::~fenPrincipale()
{
    delete ui;
}
