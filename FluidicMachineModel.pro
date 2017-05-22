#-------------------------------------------------
#
# Project created by QtCreator 2016-12-09T13:39:32
#
#-------------------------------------------------

# ensure one "debug_and_release" in CONFIG, for clarity...
debug_and_release {
    CONFIG -= debug_and_release
    CONFIG += debug_and_release
}
    # ensure one "debug" or "release" in CONFIG so they can be used as
    #   conditionals instead of writing "CONFIG(debug, debug|release)"...
CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}
CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}

QT       -= gui

TARGET = FluidicMachineModel
TEMPLATE = lib

DEFINES += FLUIDICMACHINEMODEL_LIBRARY

unix {
    target.path = /usr/lib
    INSTALLS += target
}

debug {
    QMAKE_POST_LINK=X:\fluidicMachineModel\FluidicMachineModel\setDLL.bat $$shell_path($$OUT_PWD/debug) debug

    INCLUDEPATH += X:\utils\dll_debug\include
    LIBS += -L$$quote(X:\utils\dll_debug\bin) -lutils

    INCLUDEPATH += X:\commomModel\dll_debug\include
    LIBS += -L$$quote(X:\commomModel\dll_debug\bin) -lcommonModel

    INCLUDEPATH += X:\protocolGraph\dll_debug\include
    LIBS += -L$$quote(X:\protocolGraph\dll_debug\bin) -lprotocolGraph
}

!debug {
    QMAKE_POST_LINK=X:\fluidicMachineModel\FluidicMachineModel\setDLL.bat $$shell_path($$OUT_PWD/release) release

    INCLUDEPATH += X:\utils\dll_release\include
    LIBS += -L$$quote(X:\utils\dll_release\bin) -lutils

    INCLUDEPATH += X:\commomModel\dll_release\include
    LIBS += -L$$quote(X:\commomModel\dll_release\bin) -lcommonModel

    INCLUDEPATH += X:\protocolGraph\dll_release\include
    LIBS += -L$$quote(X:\protocolGraph\dll_release\bin) -lprotocolGraph
}

INCLUDEPATH += X:\libraries\cereal-1.2.2\include

HEADERS += \
    fluidicmachinemodel/constraintssolverinterface/routingengine.h \
    fluidicmachinemodel/constraintssolverinterface/translationstack.h \
    fluidicmachinemodel/fluidicedge/tubeedge.h \
    fluidicmachinemodel/fluidicnode/containernode.h \
    fluidicmachinemodel/fluidicnode/fluidicmachinenode.h \
    fluidicmachinemodel/fluidicnode/pumpnode.h \
    fluidicmachinemodel/fluidicnode/valvenode.h \
    fluidicmachinemodel/machine_graph_utils/rules_generation/commomrulesoperations.h \
    fluidicmachinemodel/machine_graph_utils/rules_generation/domaingenerator.h \
    fluidicmachinemodel/machine_graph_utils/rules_generation/label.h \
    fluidicmachinemodel/machine_graph_utils/rules_generation/labelcombinationsiterator.h \
    fluidicmachinemodel/machine_graph_utils/rules_generation/shortstatepredicategenerator.h \
    fluidicmachinemodel/machine_graph_utils/graphrulesgenerator.h \
    fluidicmachinemodel/machine_graph_utils/machineflow.h \
    fluidicmachinemodel/machine_graph_utils/machinegraphiterator.h \
    fluidicmachinemodel/machine_graph_utils/machinegraphpressureiterator.h \
    fluidicmachinemodel/machine_graph_utils/machinestate.h \
    fluidicmachinemodel/machine_graph_utils/variablenominator.h \
    fluidicmachinemodel/rules/arithmetic/arithmeticoperable.h \
    fluidicmachinemodel/rules/arithmetic/binaryoperation.h \
    fluidicmachinemodel/rules/arithmetic/integernumber.h \
    fluidicmachinemodel/rules/arithmetic/unaryoperation.h \
    fluidicmachinemodel/rules/arithmetic/variable.h \
    fluidicmachinemodel/rules/conjunction.h \
    fluidicmachinemodel/rules/equality.h \
    fluidicmachinemodel/rules/predicate.h \
    fluidicmachinemodel/rules/rule.h \
    fluidicmachinemodel/rules/variabledomain.h \
    fluidicmachinemodel/fluidicmachinemodel.h \
    fluidicmachinemodel/fluidicmachinemodel_global.h \
    fluidicmachinemodel/machinegraph.h

SOURCES += \
    fluidicmachinemodel/fluidicedge/tubeedge.cpp \
    fluidicmachinemodel/fluidicnode/containernode.cpp \
    fluidicmachinemodel/fluidicnode/fluidicmachinenode.cpp \
    fluidicmachinemodel/fluidicnode/pumpnode.cpp \
    fluidicmachinemodel/fluidicnode/valvenode.cpp \
    fluidicmachinemodel/machine_graph_utils/rules_generation/commomrulesoperations.cpp \
    fluidicmachinemodel/machine_graph_utils/rules_generation/domaingenerator.cpp \
    fluidicmachinemodel/machine_graph_utils/rules_generation/labelcombinationsiterator.cpp \
    fluidicmachinemodel/machine_graph_utils/rules_generation/shortstatepredicategenerator.cpp \
    fluidicmachinemodel/machine_graph_utils/graphrulesgenerator.cpp \
    fluidicmachinemodel/machine_graph_utils/machineflow.cpp \
    fluidicmachinemodel/machine_graph_utils/machinegraphiterator.cpp \
    fluidicmachinemodel/machine_graph_utils/machinegraphpressureiterator.cpp \
    fluidicmachinemodel/machine_graph_utils/machinestate.cpp \
    fluidicmachinemodel/machine_graph_utils/variablenominator.cpp \
    fluidicmachinemodel/rules/arithmetic/binaryoperation.cpp \
    fluidicmachinemodel/rules/arithmetic/integernumber.cpp \
    fluidicmachinemodel/rules/arithmetic/unaryoperation.cpp \
    fluidicmachinemodel/rules/arithmetic/variable.cpp \
    fluidicmachinemodel/rules/conjunction.cpp \
    fluidicmachinemodel/rules/equality.cpp \
    fluidicmachinemodel/rules/variabledomain.cpp \
    fluidicmachinemodel/fluidicmachinemodel.cpp \
    fluidicmachinemodel/machinegraph.cpp
