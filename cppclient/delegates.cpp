#include <QComboBox>
#include <QString>
#include <QDateEdit>

#include "delegates.h"

QItemDelegate* SelectDelegate(QString choice, QStringList choices, QMainWindow *parent) {

    static QMap<QString, int> choiceMap;
    choiceMap.insert("BOOL", 0);
    choiceMap.insert("CHOICE", 1);
    choiceMap.insert("TIME", 2);
    choiceMap.insert("DATE", 3);

    int val = choiceMap.value(choice, -1);

    switch (val) {
    case 0:      // BOOL
    case 1:      // CHOICE
        return new ComboDelegate(choices, parent);
    case 2:      // TIME
        return new TimeDelegate(parent);
    case 3:      // DATE
        return new DateDelegate(parent);
    default:
        return new QItemDelegate(parent);
    }
}

QWidget* ComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&,
                                     const QModelIndex&) const
{
    QComboBox *choiceEdit = new QComboBox(parent);
    choiceEdit->addItems(data);
    return choiceEdit;
}

QWidget* TimeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&,
                                     const QModelIndex&) const
{
    QTimeEdit *t = new QTimeEdit(parent);
    return t;
}

QWidget* DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&,
                                     const QModelIndex&) const
{
    QDateEdit *d = new QDateEdit(parent);
    d->setCalendarPopup(true);
    return d;
}

// We want to show the pure date i.e. "2012-11-02" rather than the default
// "2012-11-02-08:00".
void DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    model->setData(index, ((QDateEdit*) editor)->date());
}
