QT += core gui network

unix {
	QMAKE_CXXFLAGS += -std=c++11
}

TARGET = freepoint
TEMPLATE = app

SOURCES += \
         cxn_setup.cpp \
         add_new_row.cpp \
         jsonpackets.cpp \
         settings.cpp \
         delegates.cpp \
         table_tools.cpp \
         main.cpp\
         mainwindow.cpp \
         database.cpp \
         login.cpp

HEADERS  += \
	     cxn_setup.h \
         add_new_row.h \
         delegates.h \
         jsonpackets.h \
         table_tools.h \
         login.h \
         mainwindow.h \
         database.h \
         settings.h \

unix:RESOURCES += "../resources/resource.qrc"
unix:INCLUDEPATH += "/usr/include/"
unix:LIBS += "-lqjson"
unix {
    FORMS += \
    ../ui/add_new_row.ui \
    ../ui/connections.ui \
    ../ui/create_new_table.ui \
    ../ui/loginbox.ui \
    ../ui/mainwindow.ui \
    ../ui/cxn_setup.ui
}

win32:RESOURCES += "..\resources\resource.qrc"
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
    ..\ui\cxn_setup.ui
}
