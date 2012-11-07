#include "server_setup.h"
#include "jsonpackets.h"
#include "table_tools.h"

ServerSetup::ServerSetup(MainWindow *parent)
    : QDialog(parent), parent(parent), ui(new Ui_CXNServer) {
    ui->setupUi(this);
}

void ServerSetup::accept() {

    WriteJSONServerFile(ui->txtBaseURL->text(), ui->txtLoginURL->text(),
                        ui->txtQueryURL->text(), ui->txtUpdateURL->text(),
                        ui->txtInsertURL->text(), ui->txtDeleteURL->text(),
                        ui->txtCreateURL->text(), appendDir(sgetcwd(), "server.json").path());
    QDialog::accept();
}

void ServerSetup::reject() {
    exit(-1);
}
