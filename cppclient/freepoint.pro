QT += core gui network

unix {
	QMAKE_CXXFLAGS += -std=c++11
}

TARGET = freepoint
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        database.cpp \
        login.cpp

HEADERS  += login.h \
         mainwindow.h \
         database.h \
         settings.h

unix:INCLUDEPATH += "/usr/include/"
unix:LIBS += "-lqjson"
unix {
    FORMS += \
    ../ui/add_new_row.ui \
    ../ui/connections.ui \
    ../ui/create_new_table.ui \
    ../ui/loginbox.ui \
    ../ui/mainwindow.ui \
    ../ui/sql_setup.ui
}

win32:INCLUDEPATH += C:\include\qjson\include
win32:INCLUDEPATH += C:\include\qjson\src
win32:LIBS += -LC:\include\qjson\lib -lqjson0
win32 {
    FORMS += \
    ..\ui\add_new_row.ui \
    ..\ui\connections.ui \
    ..\ui\create_new_table.ui \
    ..\ui\loginbox.ui \
    ..\ui\mainwindow.ui \
    ..\ui\sql_setup.ui
}
