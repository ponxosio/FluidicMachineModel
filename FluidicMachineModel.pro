#-------------------------------------------------
#
# Project created by QtCreator 2016-12-09T13:39:32
#
#-------------------------------------------------

QT       -= gui

TARGET = FluidicMachineModel
TEMPLATE = lib

DEFINES += FLUIDICMACHINEMODEL_LIBRARY

SOURCES += fluidicmachinemodel.cpp \
    rules/arithmetic/variable.cpp \
    rules/arithmetic/binaryoperation.cpp

HEADERS += fluidicmachinemodel.h\
        fluidicmachinemodel_global.h \
    rules/rule.h \
    rules/arithmetic/arithmeticoperable.h \
    rules/arithmetic/variable.h \
    rules/arithmetic/binaryoperation.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += X:\fluidicMachineModel\includes
