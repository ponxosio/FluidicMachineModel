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

SOURCES += fluidicmachinemodel.cpp \
    rules/equality.cpp \
    rules/conjunction.cpp \
    rules/arithmetic/binaryoperation.cpp \
    rules/arithmetic/integernumber.cpp \
    rules/arithmetic/variable.cpp \
    fluidicedge/tubeedge.cpp \
    fluidicnode/fluidicmachinenode.cpp \
    fluidicnode/pumpnode.cpp \
    fluidicnode/valvenode.cpp \
    fluidicnode/containernode.cpp \
    machine_graph_utils/machinestate.cpp \
    machine_graph_utils/machineflow.cpp \
    rules/variabledomain.cpp \
    rules/arithmetic/unaryoperation.cpp \
    machinegraph.cpp \
    machine_graph_utils/variablenominator.cpp \
    machine_graph_utils/graphrulesgenerator.cpp \
    machine_graph_utils/rules_generation/commomrulesoperations.cpp \
    machine_graph_utils/rules_generation/domaingenerator.cpp \
    machine_graph_utils/rules_generation/shortstatepredicategenerator.cpp \
    machine_graph_utils/rules_generation/labelcombinationsiterator.cpp \
    machine_graph_utils/machinegraphiterator.cpp \
    machine_graph_utils/machinegraphpressureiterator.cpp \
    plugininterface/pluginconfiguration.cpp

HEADERS += fluidicmachinemodel.h\
        fluidicmachinemodel_global.h \
    rules/rule.h \
    rules/arithmetic/arithmeticoperable.h \
    rules/predicate.h \
    rules/equality.h \
    rules/conjunction.h \
    rules/arithmetic/binaryoperation.h \
    rules/arithmetic/integernumber.h \
    rules/arithmetic/variable.h \
    fluidicedge/tubeedge.h \
    fluidicnode/fluidicmachinenode.h \
    fluidicnode/pumpnode.h \
    fluidicnode/valvenode.h \
    fluidicnode/containernode.h \
    machine_graph_utils/machinestate.h \
    machine_graph_utils/machineflow.h \
    rules/variabledomain.h \
    rules/arithmetic/unaryoperation.h \
    machinegraph.h \
    machine_graph_utils/variablenominator.h \
    machine_graph_utils/graphrulesgenerator.h \
    machine_graph_utils/rules_generation/commomrulesoperations.h \
    machine_graph_utils/rules_generation/domaingenerator.h \
    machine_graph_utils/rules_generation/shortstatepredicategenerator.h \
    machine_graph_utils/rules_generation/label.h \
    machine_graph_utils/rules_generation/labelcombinationsiterator.h \
    machine_graph_utils/machinegraphiterator.h \
    machine_graph_utils/machinegraphpressureiterator.h \
    constraintssolverinterface/routingengine.h \
    constraintssolverinterface/translationstack.h \
    plugininterface/heaterpluginproduct.h \
    plugininterface/lightpluginproduct.h \
    plugininterface/odsensorpluginproduct.h \
    plugininterface/pluginabstractfactory.h \
    plugininterface/pluginconfiguration.h \
    plugininterface/pumppluginproduct.h \
    plugininterface/stirerpluginproduct.h \
    plugininterface/valvepluginproduct.h

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
