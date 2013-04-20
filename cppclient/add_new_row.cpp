#include "mainwindow.h"
#include "add_new_row.h"

AddNewRow::AddNewRow(QStringList headers, QList<QItemDelegate*> delegates, MainWindow *parent)
    : QDialog(parent), parent(parent), ui(new Ui_AddNewRow),
      currentNam(nullptr)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->insertRow(0);
    for (int x = 0; x < delegates.size(); ++x)
        ui->tableWidget->setItemDelegateForColumn(x, delegates[x]);
    for (int x = 0; x < headers.size(); ++x)
        ui->tableWidget->setItem(0, x, new QTableWidgetItem(""));
}

void AddNewRow::accept() {
    int colcount = ui->tableWidget->columnCount();
    QStringList newrowdata;
    newrowdata.reserve(colcount);
    newrowdata.append("NULL");
    for (int x = 1; x < colcount ; ++x) {
        QTableWidgetItem* cell = ui->tableWidget->item(0, x);
        if (!cell)
            newrowdata.append(QString(""));
        else
            newrowdata.append(cell->text());

    }
    parent->InsertRow(newrowdata);
    QDialog::accept();
}
