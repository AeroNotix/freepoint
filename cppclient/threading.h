#ifndef THREADING_H
#define THREADING_H

#include <QRunnable>
#include <QObject>
#include <QThread>
#include "mainwindow.h"

class RowDeleter : public QThread
{
    Q_OBJECT

public:
    explicit RowDeleter(QWidget *parent, int rows, int cols);
    void run();

private:
    QWidget *parent;
    int rows;
    int cols;

signals:
    void DeleteItemSIG(int, int);
    void DeleteRowSIG(int);
};

class RowInserter
    : public QRunnable {
public:
    void run();
};

class ItemDeleter
    : public QRunnable {
public:
    void run();
};

class ItemInserter
    : public QRunnable {
public:
    void run();
};

#endif
