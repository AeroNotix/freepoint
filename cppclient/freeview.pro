QT       += core gui network
QMAKE_CXXFLAGS += -std=c++11

TARGET = freeview
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        database.cpp \
        login.cpp

HEADERS  += login.h \
         mainwindow.h \
         database.h \
         settings.h

unix {
    FORMS += \
    ../ui/add_new_row.ui \
    ../ui/connections.ui \
    ../ui/create_new_table.ui \
    ../ui/loginbox.ui \
    ../ui/mainwindow.ui \
    ../ui/sql_setup.ui
}

unix:INCLUDEPATH += "/usr/include/"
unix:LIBS += "-lqjson"

win32 {
    FORMS += \
    ..\ui\add_new_row.ui \
    ..\ui\connections.ui \
    ..\ui\create_new_table.ui \
    ..\ui\loginbox.ui \
    ..\ui\mainwindow.ui \
    ..\ui\sql_setup.ui
}
