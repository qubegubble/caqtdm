include (../../../caQtDM_Viewer/qtdefs.pri)
QT += core gui opcua
contains(QT_VER_MAJ, 5) {
    QT     += widgets
}
contains(QT_VER_MAJ, 6) {
    QT     += widgets
}

CONFIG += warn_on
CONFIG += release
CONFIG += demo_plugin
include (../../../caQtDM.pri)

MOC_DIR = ./moc
VPATH += ./src

TEMPLATE        = lib
CONFIG         += plugin
INCLUDEPATH    += .
INCLUDEPATH    += ../
INCLUDEPATH    += ../../src
HEADERS         = ../controlsinterface.h \
    opcua_client.h \
    opcua_plugin.h
SOURCES         = \
    opcua_client.cpp \
    opcua_plugin.cpp
TARGET          = demo_plugin
android {
   INCLUDEPATH += $(ANDROIDFUNCTIONSINCLUDE)
}
