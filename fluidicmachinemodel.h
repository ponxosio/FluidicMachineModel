#ifndef FLUIDICMACHINEMODEL_H
#define FLUIDICMACHINEMODEL_H

#define UNDEFINE_VAR_STATUS -1

#include <bitset>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "machinestate.h"
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
                        short int ratePrecisionDecimal = 2);
    virtual ~FluidicMachineModel();

    void loadContainer(short int id, float volume) throw(std::invalid_argument);
    void setContinuousFlow(short int idStart, short int idEnd, float flowRate,
                           const std::vector<short int> * intermediateContainers = NULL,
                           const std::vector<std::tuple<short int, short int, bool>> * intermediateTubes = NULL) throw(std::invalid_argument);
    void stopContinuousFlow(short int idStart, short int idEnd, float flowRate,
                            const std::vector<short int> * intermediateContainers = NULL,
                            const std::vector<std::tuple<short int, short int, bool>> * intermediateTubes = NULL) throw(std::invalid_argument);

    void setTranslationStack(std::shared_ptr<TranslationStack> translationStack);
    void updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory);

    void calculateNewRoute() throw(std::runtime_error);

protected:
    short int openContainers;
    short int ratePrecisionInteger;
    short int ratePrecisionDecimal;

    MachineState containersTubesInterfaceState;
    MachineState actualFullMachineState;
    std::unordered_map<short int,short int> idOpenContainerLiquidIdMap;

    MachineGraph graph;
    std::shared_ptr<TranslationStack> translationStack;
    std::vector<std::shared_ptr<Rule>> rules;
    std::shared_ptr<RoutingEngine> routingEngine;

    void analizeGraph();

    short int transformRateToInt(float rate);
    float transformRateToDoucble(short int rate);

    void addToContainersState(short int idContainerId, long long state) throw(std::invalid_argument);
    void substractToContainersState(short int idContainer, long long state) throw(std::invalid_argument);
    void addToTubeState(short int idSource, short int idTarget, long long state) throw(std::invalid_argument);
    void substractToTubeState(short int idSource, short int idTarget, long long state) throw(std::invalid_argument);

    int getLiquidId(short int state);
    int addLiquidIds(short int id1, short int id2) throw(std::invalid_argument);
    int substractLiquidIds(short int id1, short int id2) throw(std::invalid_argument);
};

#endif // FLUIDICMACHINEMODEL_H
