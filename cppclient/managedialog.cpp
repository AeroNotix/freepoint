#include "managedialog.h"
#include "jsonpackets.h"
#include "table_tools.h"

ManageConnectionDialog::ManageConnectionDialog(MainWindow *parent)
    : QDialog(parent), parent(parent),
      ui(new Ui_ConnectionsDialog) {
    ui->setupUi(this);
    fillList();
};

void ManageConnectionDialog::accept() {
    QDialog::accept();
}

void ManageConnectionDialog::reject() {
    if (AreYouSure())
        QDialog::reject();
}

void ManageConnectionDialog::fillList() {
    QStringList rownames = parent->GetHeadings();
    for (int x = 0; x < rownames.size(); ++x) {
        std::pair<QString, QString> dbinfo = parent->GetDBTableInfo(rownames[x]);
        CustomListWidget *newitem = new CustomListWidget(dbinfo.first + "." + dbinfo.second);
        newitem->setObjectName(rownames[x]);
        ui->listWidget->addItem(newitem);
    }
}

void ManageConnectionDialog::populateFields(QListWidgetItem *item) {
    if (!item)
        return;

    QStringList dbandtable = item->text().split(".");
    if (dbandtable.size() != 2)
        return;
    ui->txt_database->setText(dbandtable[0]);
    ui->txt_table->setText(dbandtable[1]);
}

void ManageConnectionDialog::deleteRow() {
    if (!AreYouSure())
        return;
    QListWidgetItem *curr = ui->listWidget->currentItem();
    if (!curr)
        return;
    ui->listWidget->takeItem(ui->listWidget->row(curr));
    WriteConfig();
}

void ManageConnectionDialog::WriteConfig() {
    QStringList rownames;
    for (int x = 0; x < ui->listWidget->count(); ++x) {
        QString objname = ((CustomListWidget*) ui->listWidget->item(x))->objectName();
        rownames.append(objname);
    }
    WriteJSONConfigFile(rownames, parent->GetMap(), "config.json");
}
