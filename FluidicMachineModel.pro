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
    rules/equality.cpp \
    rules/conjunction.cpp \
    rules/arithmetic/binaryoperation.cpp \
    rules/arithmetic/integernumber.cpp \
    rules/arithmetic/variable.cpp \
    util/AutoEnumerate.cpp \
    util/Utils.cpp \
    fluidicedge/tubeedge.cpp \
    fluidicnode/fluidicmachinenode.cpp \
    fluidicnode/pumpnode.cpp \
    fluidicnode/valvenode.cpp \
    fluidicnode/functions/functionset.cpp \
    plugininterface/pluginconfiguration.cpp \
    fluidicnode/functions/valvepluginroutefunction.cpp \
    fluidicnode/functions/pumppluginfunction.cpp \
    fluidicnode/functions/measureodfunction.cpp \
    fluidicnode/functions/heatfunction.cpp \
    fluidicnode/functions/lightfunction.cpp \
    fluidicnode/functions/stirfunction.cpp \
    fluidicnode/containernode.cpp \
    machine_graph_utils/machinestate.cpp \
    machine_graph_utils/machineflow.cpp \
    rules/variabledomain.cpp \
    rules/arithmetic/unaryoperation.cpp \
    util/sequence.cpp \
    machinegraph.cpp \
    machine_graph_utils/variablenominator.cpp \
    machine_graph_utils/graphrulesgenerator.cpp \
    machine_graph_utils/rules_generation/commomrulesoperations.cpp \
    machine_graph_utils/rules_generation/domaingenerator.cpp \
    machine_graph_utils/rules_generation/shortstatepredicategenerator.cpp \
    machine_graph_utils/rules_generation/labelcombinationsiterator.cpp \
    machine_graph_utils/machinegraphiterator.cpp \
    machine_graph_utils/machinegraphpressureiterator.cpp

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
    graph/Edge.h \
    graph/Graph.h \
    graph/Node.h \
    util/AutoEnumerate.h \
    util/Utils.h \
    fluidicedge/tubeedge.h \
    fluidicnode/fluidicmachinenode.h \
    fluidicnode/pumpnode.h \
    constraintssolverinterface/translationstack.h \
    plugininterface/pluginabstractfactory.h \
    plugininterface/valvepluginproduct.h \
    plugininterface/pumppluginproduct.h \
    plugininterface/heaterpluginproduct.h \
    plugininterface/lightpluginproduct.h \
    plugininterface/stirerpluginproduct.h \
    plugininterface/odsensorpluginproduct.h \
    fluidicnode/valvenode.h \
    fluidicnode/functions/function.h \
    fluidicnode/functions/functionset.h \
    plugininterface/pluginconfiguration.h \
    fluidicnode/functions/valvepluginroutefunction.h \
    fluidicnode/functions/pumppluginfunction.h \
    fluidicnode/functions/measureodfunction.h \
    fluidicnode/functions/heatfunction.h \
    fluidicnode/functions/lightfunction.h \
    fluidicnode/functions/stirfunction.h \
    fluidicnode/containernode.h \
    constraintssolverinterface/routingengine.h \
    machine_graph_utils/machinestate.h \
    machine_graph_utils/machineflow.h \
    rules/variabledomain.h \
    rules/arithmetic/unaryoperation.h \
    util/sequence.h \
    machinegraph.h \
    machine_graph_utils/variablenominator.h \
    machine_graph_utils/graphrulesgenerator.h \
    machine_graph_utils/rules_generation/commomrulesoperations.h \
    machine_graph_utils/rules_generation/domaingenerator.h \
    machine_graph_utils/rules_generation/shortstatepredicategenerator.h \
    machine_graph_utils/rules_generation/label.h \
    machine_graph_utils/rules_generation/labelcombinationsiterator.h \
    machine_graph_utils/machinegraphiterator.h \
    machine_graph_utils/machinegraphpressureiterator.h

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
