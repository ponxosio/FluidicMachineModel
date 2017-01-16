#ifndef FLUIDICMACHINEMODEL_H
#define FLUIDICMACHINEMODEL_H

#define UNDEFINE_VAR_STATUS -1
#define LONG_LONG_DIGITS_SIZE 18

#include <bitset>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "machinestate.h"
#include "machineflow.h"
#include "constraintssolverinterface/translationstack.h"
#include "constraintssolverinterface/routingengine.h"
#include "graph/Graph.h"
#include "fluidicnode/fluidicmachinenode.h"
#include "fluidicnode/containernode.h"
#include "fluidicnode/pumpnode.h"
#include "fluidicnode/valvenode.h"
#include "fluidicedge/tubeedge.h"
#include "plugininterface/pluginabstractfactory.h"
#include "rules/rule.h"

#include "fluidicmachinemodel_global.h"

class FLUIDICMACHINEMODELSHARED_EXPORT FluidicMachineModel
{

    typedef std::shared_ptr<Graph<FluidicMachineNode, TubeEdge>> MachineGraph;
public:

    FluidicMachineModel(MachineGraph graph,
                        std::shared_ptr<TranslationStack> translationStack = std::shared_ptr<TranslationStack>(),
                        short int ratePrecisionInteger = 3,
                        short int ratePrecisionDecimal = 2) throw(std::overflow_error);
    virtual ~FluidicMachineModel();

    void loadContainer(short int id, float volume) throw(std::invalid_argument);
    void setContinuousFlow(short int idStart, short int idEnd, float flowRate) throw(std::invalid_argument);
    void stopContinuousFlow(short int idStart, short int idEnd) throw(std::invalid_argument);

    void setTranslationStack(std::shared_ptr<TranslationStack> translationStack);
    void updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory);

    void calculateNewRoute() throw(std::runtime_error);

protected:
    short int openContainers;
    short int maxOpenContainer;

    MachineGraph graph;

    MachineState actualFullMachineState;
    MachineFlow flowEngine;
    std::unordered_map<short int,short int> idOpenContainerLiquidIdMap;

    std::shared_ptr<TranslationStack> translationStack;
    std::vector<std::shared_ptr<Rule>> rules;
    std::shared_ptr<RoutingEngine> routingEngine;

    std::shared_ptr<RoutingEngine> translateRules() throw(std::runtime_error);
    void analizeGraph() throw(std::overflow_error);
    short int calculateMaxOpenContainers(short int ratePrecisionDecimal, short int ratePrecisionInteger);

    void addStack2State(const std::deque<short int> & queue, float rate, MachineState & state) throw(std::invalid_argument);
    void sendState2components(const MachineState & state) throw(std::runtime_error);
};

#endif // FLUIDICMACHINEMODEL_H
