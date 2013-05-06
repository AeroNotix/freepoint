#include "mainwindow.h"
#include "add_new_row.h"

/**
   Constructor.

   @param[headers A QStringList of headers which will populate the
                  table's column labels.
   @param[delegates Delegates is a list of QItemDelegate* which will be
                    used as the editing widgets for the dialog.
   @param[parent Our parent which invoked this dialog.
 */
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

/**
   Accepts the dialog

   When the user is done editing they should press this to submit
   their changes to the database.
*/
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
