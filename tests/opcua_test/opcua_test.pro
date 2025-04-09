

QT += testlib
QT -= gui
QT += opcua

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += opcua_test.cpp

HEADERS += \
    opcua_test.h

LIBS += -L$$OUT_PWD/../../caQtDM_Lib/caQtDM_Plugins/opcua_core -lopcua_core
INCLUDEPATH += $$PWD/../../caQtDM_Lib/caQtDM_Plugins/opcua_core

LIBS += -L/usr/lib/x86_64-linux-gnu/qt5/plugins/opcua/ -lopen62541
INCLUDEPATH += /usr/include/open62541
# Optional: if you're using headers from other places
DEPENDPATH += $$PWD/../../caQtDM_Lib/caQtDM_Plugins/opcua_core

