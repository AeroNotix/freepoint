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
    explicit ComboDelegate(QStringList data, QMainWindow *parent = nullptr)
        : QItemDelegate(parent), data(data) {};
    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;

protected:
    QStringList data;

};

class ChoiceDelegate :
    public ComboDelegate {

    Q_OBJECT

public:
    explicit ChoiceDelegate(QStringList data, QMainWindow *parent = nullptr)
        : ComboDelegate(data, parent) {};
};

class TimeDelegate :
    public QItemDelegate {

    Q_OBJECT

public:
    explicit TimeDelegate(QMainWindow *parent = nullptr)
        : QItemDelegate(parent) {};
};

class DateDelegate :
    public QItemDelegate {

    Q_OBJECT

public:
    explicit DateDelegate(QMainWindow *parent = nullptr)
        : QItemDelegate(parent) {};
    QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;
};

QItemDelegate* SelectDelegate(QString choice, QMainWindow *parent = nullptr);
#endif // DELEGATES_H
