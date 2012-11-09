#include <stdexcept>

#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QMessageBox>
#include "createnewdatabase.h"

CreateNewDatabase::CreateNewDatabase(MainWindow *p)
    : QDialog(p), parent(p), ui(new Ui_CreateNewDatabase),
      rowmap(new QMap<QString, QString>()), column_number(-1),
      list_items(QList<QListWidgetItem*>())
{
    ui->setupUi(this);
}

/*
  TODO: Refactor this method.

  accept creates the JSON string from the user-created rows and fires
  off a request to the database.
*/
void CreateNewDatabase::accept() {
    QList<QString> keys = rowmap->keys();
    QString ss;
    QString sa;
    QString dq = "\"";
    QTextStream s(&ss);
    QTextStream sub(&sa);

    s << "{" << dq << "DATABASE" << dq << ":";
    s << dq << ui->txt_database_name->text() << dq << ",";
    s << dq << "TABLE" << dq << ":";
    s << dq << ui->txt_table_name->text() << dq << ",";
    sub << dq << "HEADINGS" << dq << ":{";

    for (int x = 0; x < keys.size(); ++x) {
        sub << (*rowmap)[keys[x]];
        if (x + 1 != keys.size())
            sub << ",";
    }
    s << *sub.string() << "}}";
    s << "," << dq << "PAYLOAD" << dq << ":" << dq << "{";
    s << (*sub.string()).replace("\"", "\\\"") << "}}" << '"';

    parent->CreateNew(*s.string());
    QDialog::accept();
}

/*
  reject just closes the dialog without using the suppled data
*/
void CreateNewDatabase::reject() {
    QMessageBox *msgbox = new QMessageBox(this);
    msgbox->setText("Are you sure?");
    msgbox->addButton(QMessageBox::Yes);
    msgbox->addButton(QMessageBox::No);
    bool ans = msgbox->exec() == QMessageBox::Yes;
    delete msgbox;
    if (ans)
        QDialog::reject();
}

/*
  changeFieldDescriptions is fired when the fieldButtonGroup is changed
  we N = (~N-1) because the field ID numbers are negative indexes starting
  from -2 for some bloody reason. Dafuq?
*/
void CreateNewDatabase::changeFieldDescriptions(int i) {
    ui->stackedWidget->setCurrentIndex(~i-1);
}

/*
  acceptFieldAdd is fired on a StackedWidget page. It will determine
  which page is currently active and create a row entry from the widgets
  on that page.
*/
void CreateNewDatabase::acceptFieldAdd() {
    QString s = GetCurrentRowname();
    if (s.size() == 0)
        return;

    if (CheckOverwrite())
        return;

    ++column_number;

    switch(ui->stackedWidget->currentIndex())
    {
    case CreateNewDatabase::text:
        rowmap->insert(ui->txt_grp_rowname->text(), generateTextData());
        break;
    case CreateNewDatabase::choice:
        rowmap->insert(ui->choice_grp_rowname->text(), generateChoiceData());
        break;
    case CreateNewDatabase::date:
        rowmap->insert(ui->date_grp_rowname->text(), generateDateData());
        break;
    case CreateNewDatabase::time:
        std::runtime_error("Adding time rows is not implemented");
        break;
    case CreateNewDatabase::group:
        std::runtime_error("Adding group rows is not implemented");
        break;
    case CreateNewDatabase::currency:
        rowmap->insert(ui->curr_grp_rowname->text(), generateCurrData());
        break;
    }
    GenericCleanup();
}

/*
  Generates the json string for the TEXT type.
*/
QString CreateNewDatabase::generateTextData() {
    std::unique_ptr<QString> ss(new QString(""));
    QString dq("\"");
    QTextStream s(ss.get());

    s << genericAddData();
    s << dq << "TYPE" << dq << ":" << dq << "VARCHAR" << dq << ",";
    s << dq << "LEN" << dq << ":" << ui->txt_grp_length->value();
    s << "}}";

    return *s.string();
}

/*
  Generates the json string for the CHOICE type.
*/
QString CreateNewDatabase::generateChoiceData() {
    std::unique_ptr<QString> ss(new QString(""));
    QString dq("\"");
    QTextStream s(ss.get());

    QStringList choices = ui->choice_grp_choices->toPlainText().split("\n");
    choices.removeDuplicates();

    s << genericAddData();
    s << dq << "TYPE" << dq << ":" << dq << "CHOICE" << dq << ",";
    s << dq << "CHOICES" << dq << ":[";
    for (int x = 0; x < choices.size(); ++x) {
        s << dq << choices[x] << dq;
        if (x + 1 != choices.size())
            s << ",";
    }
    s << "]}}";

    return *s.string();
}

/*
  Generates the json string for the CURR type.
*/
QString CreateNewDatabase::generateCurrData() {
    std::unique_ptr<QString> ss(new QString(""));
    QString dq("\"");
    QTextStream s(ss.get());

    s << genericAddData();
    s << dq << "TYPE" << dq << ":" << dq << "DATE" << dq;

    return *s.string();
}

/*
  Generates the json string for the DATE type.
*/
QString CreateNewDatabase::generateDateData() {
    std::unique_ptr<QString> ss(new QString(""));
    QString dq("\"");
    QTextStream s(ss.get());

    s << genericAddData();
    s << dq << "TYPE" << dq << ":" << dq << "DATE" << dq;
    s << "}}";

    return *s.string();
}

/*
  Adds a QListWidgetItem to the row list UI element.
*/
void CreateNewDatabase::AddToList(QString toAdd) {
    QListWidgetItem *item = new QListWidgetItem(toAdd);
    list_items.append(item);
    ui->list_db_rows->addItem(item);
}

/*
  Most pages have similar items, we clear these generic items here.
*/
void CreateNewDatabase::GenericCleanup() {
    AddToList(GetCurrentRowname());
    switch(ui->stackedWidget->currentIndex())
    {
    case CreateNewDatabase::text:
        ui->txt_grp_isnull->setCheckState(Qt::Unchecked);
        ui->txt_grp_unique->setCheckState(Qt::Unchecked);
        ui->txt_grp_length->setValue(255);
        ui->txt_grp_rowname->setText("");
        break;
    case CreateNewDatabase::choice:
        ui->choice_grp_isnull->setCheckState(Qt::Unchecked);
        ui->choice_grp_unique->setCheckState(Qt::Unchecked);
        ui->choice_grp_rowname->setText("");
        ui->choice_grp_choices->clear();
        break;
    case CreateNewDatabase::date:
        ui->date_grp_isnull->setCheckState(Qt::Unchecked);
        ui->date_grp_unique->setCheckState(Qt::Unchecked);
        ui->date_grp_rowname->setText("");
        break;
    case CreateNewDatabase::time:
        std::runtime_error("Adding time rows is not implemented");
    case CreateNewDatabase::group:
        std::runtime_error("Adding group rows is not implemented");
    case CreateNewDatabase::currency:
        ui->curr_grp_isnull->setCheckState(Qt::Unchecked);
        ui->curr_grp_unique->setCheckState(Qt::Unchecked);
        ui->curr_grp_rowname->setText("");
        break;
    }
}

/*
  Returns the textual representation of the current rowname.
 */
QString CreateNewDatabase::GetCurrentRowname() {
    switch(ui->stackedWidget->currentIndex())
    {
    case CreateNewDatabase::text:
        return ui->txt_grp_rowname->text();
    case CreateNewDatabase::choice:
        return ui->choice_grp_rowname->text();
    case CreateNewDatabase::date:
        return ui->date_grp_rowname->text();
    case CreateNewDatabase::time:
        std::runtime_error("Adding time rows is not implemented");
    case CreateNewDatabase::group:
        std::runtime_error("Adding group rows is not implemented");
    case CreateNewDatabase::currency:
        return ui->curr_grp_rowname->text();
    }
    std::runtime_error("Shouldn't get here");
    return QString("");
}

bool CreateNewDatabase::CheckOverwrite() {

    QString tocheck = GetCurrentRowname();
    if (!rowmap->contains(tocheck))
        return false;

    // FIXME: need to fix the column counting here somewhere.
    QMessageBox *msgbox = new QMessageBox(this);
    QString text = "Row Exists:";
    text.append(tocheck); text.append(". Overwrite?");
    msgbox->setText(text);
    msgbox->addButton(QMessageBox::Yes);
    msgbox->addButton(QMessageBox::No);
    bool ans = msgbox->exec() == QMessageBox::Yes;
    delete msgbox;
    return !ans;
}

/*
  Returns a boolean value as a string
*/
QString CreateNewDatabase::toStrBool(bool it_is) {
    if (it_is)
        return "true";
    return "false";
}

/*
  Most pages have similar items, we add their generic data to the JSON
  string here.
*/
QString CreateNewDatabase::genericAddData() {
    std::unique_ptr<QString> ss(new QString(""));
    QString dq("\"");
    QTextStream s(ss.get());
    bool un = false;
    bool nu = false;
    QString rowname;

    switch(ui->stackedWidget->currentIndex())
    {
    case CreateNewDatabase::text:
        rowname = ui->txt_grp_rowname->text();
        un = ui->txt_grp_unique->isChecked();
        nu = ui->txt_grp_isnull->isChecked();
        break;
    case CreateNewDatabase::choice:
        rowname = ui->choice_grp_rowname->text();
        un = ui->choice_grp_unique->isChecked();
        nu = ui->choice_grp_isnull->isChecked();
        break;
    case CreateNewDatabase::date:
        rowname = ui->date_grp_rowname->text();
        un = ui->date_grp_unique->isChecked();
        nu = ui->date_grp_isnull->isChecked();
        break;
    case CreateNewDatabase::time:
        std::runtime_error("Adding time rows is not implemented");
        break;
    case CreateNewDatabase::group:
        std::runtime_error("Adding group rows is not implemented");
        break;
    case CreateNewDatabase::currency:
        rowname = ui->curr_grp_rowname->text();
        un = ui->curr_grp_unique->isChecked();
        nu = ui->curr_grp_isnull->isChecked();
        break;
    }

    // TODO: Refactor this shit, use the quote function in the jsonpackets.h
    s << dq << rowname << dq << ":{";
    s << dq << "RowNum" << dq << ":" << column_number << ",";
    s << dq << "RowData" << dq << ":{";
    s << dq << "UNIQUE" << dq << ":" << toStrBool(un) << ",";
    s << dq << "NULL" << dq << ":" << toStrBool(nu) << ",";

    return *s.string();
}
