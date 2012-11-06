#include "cxn_setup.h"

CXNSetup::CXNSetup(MainWindow *parent)
    : parent(parent), ui(new Ui_CXNSetup)
{
    ui->setupUi(this);
}
