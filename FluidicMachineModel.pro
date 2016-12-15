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
    rules/implication.cpp \
    rules/equality.cpp \
    rules/conjunction.cpp \
    rules/arithmetic/binaryoperation.cpp \
    rules/arithmetic/integernumber.cpp \
    rules/arithmetic/variable.cpp

HEADERS += fluidicmachinemodel.h\
        fluidicmachinemodel_global.h \
    rules/rule.h \
    rules/arithmetic/arithmeticoperable.h \
    rules/predicate.h \
    rules/translationstack.h \
    rules/implication.h \
    rules/equality.h \
    rules/conjunction.h \
    rules/arithmetic/binaryoperation.h \
    rules/arithmetic/integernumber.h \
    rules/arithmetic/variable.h \
    machinerestrictions.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

debug {
    QMAKE_POST_LINK=X:\fluidicMachineModel\FluidicMachineModel\setDLL.bat $$shell_path($$OUT_PWD/debug) debug
}

!debug {
    QMAKE_POST_LINK=X:\fluidicMachineModel\FluidicMachineModel\setDLL.bat $$shell_path($$OUT_PWD/release) release
}

INCLUDEPATH += X:\fluidicMachineModel\includes
