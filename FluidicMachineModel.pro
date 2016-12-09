#-------------------------------------------------
#
# Project created by QtCreator 2016-12-09T13:39:32
#
#-------------------------------------------------

QT       -= gui

TARGET = FluidicMachineModel
TEMPLATE = lib

DEFINES += FLUIDICMACHINEMODEL_LIBRARY

SOURCES += fluidicmachinemodel.cpp

HEADERS += fluidicmachinemodel.h\
        fluidicmachinemodel_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
