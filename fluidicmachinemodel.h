#ifndef FLUIDICMACHINEMODEL_H
#define FLUIDICMACHINEMODEL_H

#define UNDEFINE_VAR_STATUS -1

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

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
    FluidicMachineModel(MachineGraph graph, std::shared_ptr<TranslationStack> translationStack);
    virtual ~FluidicMachineModel();

    void loadContainer(int id, double volume) throw(std::invalid_argument);
    void setContinuousFlow(int idSource, int idTarget, double flowRate) throw(std::invalid_argument);
    void stopContinuousFlow(int idSource, int idTarget) throw(std::invalid_argument);
    void setTubeFlow(int edgeIdSource, int edgeIdTarget, double flowRate) throw(std::invalid_argument);

    void setTranslationStack(std::shared_ptr<TranslationStack> translationStack);
    void updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory);

protected:
    MachineGraph graph;
    std::shared_ptr<TranslationStack> translationStack;
    std::vector<std::shared_ptr<Rule>> rules;
    std::shared_ptr<RoutingEngine> routingEngine;

    std::unordered_map<std::string, int> containerStatusMap;
    std::unordered_map<std::string, int> tubeStatusMap;
    std::unordered_map<std::string, std::shared_ptr<PumpNode>> pumpsPtrMap;
    std::unordered_map<std::string, std::shared_ptr<ValveNode>> valvesPtrMap;

    void createRules();
    bool calculateNewRoute(std::vector<std::tuple<std::string, int>> & newStates);
    std::vector<std::tuple<std::string, int>> calculateActualState();
    void setNewMachineState(const std::vector<std::tuple<std::string, int>> & states, double flowRate);

    inline bool checkFlowActivated(unsigned int source, unsigned int target) {
        return ((source & target) == source)
    }
};

#endif // FLUIDICMACHINEMODEL_H
