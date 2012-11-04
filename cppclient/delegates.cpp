#include <QComboBox>

#include "delegates.h"

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
