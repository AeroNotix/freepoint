#include "threading.h"

RowDeleter::RowDeleter(MainWindow *parent, int rows)
    : QThread(), rows(rows), parent(parent) {};

void RowDeleter::run() {
    for (int x = 0; x < rows; ++x)
        parent->DeleteRow(rows - x);
}
