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
    qDebug() << "Button clicked";
    QMessageBox::information(this, "Button clicked", "Button clicked");
}

fenPrincipale::~fenPrincipale()
{
    delete ui;
}
