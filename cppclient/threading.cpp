#include "threading.h"

RowDeleter::RowDeleter(QWidget *parent, int rows, int cols)
    : QThread(), parent(parent), rows(rows), cols(cols) {};

void RowDeleter::run() {
    for (int x = 0; x < rows+1; ++x) {
        for (int y = 0; y < cols; ++y) {
            emit DeleteItemSIG(x, y);
        }
        emit DeleteRowSIG(rows - x);
    }
}
