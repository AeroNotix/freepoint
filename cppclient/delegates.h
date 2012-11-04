#ifndef DELEGATES_H
#define DELEGATES_H

#include <QtGui>
#include <QItemDelegate>
#include <QStringList>

class MainWindow;

class ComboDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit ComboDelegate(QStringList data, QMainWindow *parent = 0)
        : QItemDelegate(parent), data(data) {};
    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;

protected:
    QStringList data;

};

class BooleanDelegate :
    public ComboDelegate {

    Q_OBJECT

public:
explicit BooleanDelegate(QStringList data, QMainWindow *parent = 0)
        : ComboDelegate(data, parent) {};
};

class ChoiceDelegate :
    public ComboDelegate {

    Q_OBJECT

public:
    explicit ChoiceDelegate(QStringList data, QMainWindow *parent = 0)
        : ComboDelegate(data, parent) {};
};

class TimeDelegate :
    public QItemDelegate {

    Q_OBJECT

    explicit TimeDelegate(QMainWindow *parent = 0)
        : QItemDelegate(parent) {};
};

class DateDelegate :
    public QItemDelegate {

    Q_OBJECT

    DateDelegate(QMainWindow *parent = 0)
        : QItemDelegate(parent) {};
};

QItemDelegate* SelectDelegate(QString choice, QMainWindow *parent = nullptr);
#endif // DELEGATES_H
