#ifndef THREADING_H
#define THREADING_H

#include <QThread>

#include "mainwindow.h"

class RowDeleter
    : public QThread {
public:
    RowDeleter(MainWindow *parent, int rows);
    void run();
private:
    MainWindow *parent;
    int rows;
};

class RowInserter
    : public QThread {
public:
    void run();
};

class ItemDeleter
    : public QThread {
public:
    void run();
};

class ItemInserter
    : public QThread {
public:
    void run();
};

#endif
