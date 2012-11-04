#include <QComboBox>
#include <QString>
#include <QDateEdit>

#include "delegates.h"

QItemDelegate* SelectDelegate(QString choice, QMainWindow *parent) {

	static QMap<QString, int> choiceMap;
	choiceMap.insert("BOOL", 0);
	choiceMap.insert("CHOICE", 1);
	choiceMap.insert("TIME", 2);
	choiceMap.insert("DATE", 3);

	int val = choiceMap.value(choice, -1);

	switch (val) {
	case 0:      // BOOL
	case 1:      // CHOICE
		return new ComboDelegate(QStringList({"Yes","No"}), parent);
	case 2:      // TIME
		return new TimeDelegate(parent);
	case 3:      // DATE
		return new DateDelegate(parent);
	default:
		return new QItemDelegate(parent);
	}
}

QWidget* ComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    QComboBox *choiceEdit = new QComboBox(parent);
    choiceEdit->addItems(data);
    return choiceEdit;
}

QWidget* DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
									const QModelIndex &index) const {
	QDateEdit *d = new QDateEdit(parent);
	d->setCalendarPopup(true);
	return d;
}
