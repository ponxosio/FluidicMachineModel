#ifndef MACHINEGRAPH_H
#define MACHINEGRAPH_H

#include <memory>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "graph/Graph.h"
#include "fluidicedge/tubeedge.h"
#include "fluidicnode/fluidicmachinenode.h"
#include "fluidicnode/pumpnode.h"
#include "fluidicnode/valvenode.h"
#include "fluidicnode/containernode.h"
#include "fluidicnode/functions/function.h"
#include "plugininterface/pluginabstractfactory.h"
#include "util/AutoEnumerate.h"
#include "util/Utils.h"

#include "fluidicmachinemodel_global.h"

class MACHINEGRAPH_EXPORT MachineGraph
{
    friend class MachineGraphGenerator;

    typedef std::unordered_map<int, std::shared_ptr<int>> ColorMap;

public:
    typedef Graph<FluidicMachineNode,TubeEdge> GraphType;

    MachineGraph();
    MachineGraph(const MachineGraph & mg);
    virtual ~MachineGraph();

    void connectNodes(int idSource, int idTarget, int sourcePinToConnect, int targetPinToConnect) throw(std::invalid_argument);
    GraphType::EdgeVectorPtr getArrivingTubes(int nodeId) throw(std::invalid_argument);
    GraphType::EdgeVectorPtr getLeavingTubes(int nodeId) throw(std::invalid_argument);
    GraphType::EdgeTypePtr getTube(int idSource, int idTarget) throw(std::invalid_argument);
    GraphType::EdgeTypePtr getTubeConnectedToPin(int nodeId, int pinNumber) throw(std::invalid_argument);

    void cutTube(int idSource, int idTarget) throw(std::invalid_argument);
    void uncutTube(int idSource, int idTarget) throw(std::invalid_argument);
    void cutAllTubesConnectedTo(int idNode) throw(std::invalid_argument);
    void uncutAllTubesConnectedTo(int idNode) throw(std::invalid_argument);
    bool isTubeCutted(int idSource, int idTarget) throw(std::invalid_argument);

    void setTubeDirection(int idSource, int idTarget, TubeEdge::TubeDirection dir) throw(std::invalid_argument);
    void setAllTubesDirectionsConnectedTo(int idNode, TubeEdge::TubeDirection dir) throw(std::invalid_argument);
    TubeEdge::TubeDirection getTubeDirection(int idSource, int idTarget) throw(std::invalid_argument);

    void addNode(std::shared_ptr<FluidicMachineNode> node) throw(std::invalid_argument);
    int emplaceContainer(int numPins, ContainerType type, double maxVolume);
    int emplacePump(int numPins, PumpNode::PumpType type, std::shared_ptr<Function> pumpFunction) throw(std::invalid_argument);
    int emplaceValve(int numPins, const ValveNode::TruthTable & truthTable,
                  std::shared_ptr<Function> valveRouteFunction) throw(std::invalid_argument);

    bool hasNode(int id);
    std::shared_ptr<FluidicMachineNode> getNode(int id) throw(std::invalid_argument);
    std::shared_ptr<ContainerNode> getContainer(int id) throw(std::invalid_argument);
    std::shared_ptr<FluidicMachineNode> getFirstNodeOnlyLeavingTubes() throw(std::invalid_argument);
    std::shared_ptr<PumpNode> getPump(int id) throw(std::invalid_argument);
    std::shared_ptr<ValveNode> getValve(int id) throw(std::invalid_argument);

    const std::vector<std::shared_ptr<MachineGraph>> & getConnectedComponents();

    void updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory);

    inline bool isContainer(int id) {
        return (isOpenContainer(id) || isCloseContainer(id));
    }
    inline bool isOpenContainer(int id)  {
        return (openContainersSet.find(id) != openContainersSet.end());
    }
    inline bool isCloseContainer(int id) {
        return (closeContainersSet.find(id) != closeContainersSet.end());
    }
    inline bool isPump(int id) {
        return (pumpsSet.find(id) != pumpsSet.end());
    }
    inline bool isValve(int id) {
        return (valvesSet.find(id) != valvesSet.end());
    }
    inline bool areConnected(int idSource, int idTarget) {
        return graphPtr->areConnected(idSource, idTarget);
    }
    inline const unordered_set<int> & getPumpsIdsSet() {
        return pumpsSet;
    }
    inline const unordered_set<int> & getValvesIdsSet() {
        return valvesSet;
    }
    inline const unordered_set<int> & getOpenContainersIdsSet() {
        return openContainersSet;
    }
    inline const unordered_set<int> & getCloseContainersIdsSet() {
        return closeContainersSet;
    }
    inline const unordered_map<short int, short int> & getOpenContainerLiquidIdMap() {
        return openContainerLiquidIdMap;
    }

    inline int getNumOpenContainers() {
        return openContainersSet.size();
    }
    inline GraphType::UniqueEdgeMapPtr getAlledgesMap() {
        return graphPtr->getAllEdges();
    }
    inline GraphType::NodeMapPtr getAllNodesMap() {
        return graphPtr->getAllNodes();
    }

    inline std::string toString() {
        return graphPtr->toString();
    }

protected:
    std::shared_ptr<GraphType> graphPtr;
    bool connectedComponentsUpdated;
    std::vector<std::shared_ptr<MachineGraph>> connectedComponentsVector;

    AutoEnumerate sequence;
    std::unordered_set<int> pumpsSet;
    std::unordered_set<int> valvesSet;
    std::unordered_set<int> closeContainersSet;
    std::unordered_set<int> openContainersSet;

    AutoEnumerate luquidIdserie;
    std::unordered_map<short int, short int> openContainerLiquidIdMap;


    static bool valveNeedProcess(int valveId, shared_ptr<MachineGraph> component);

    void calculateConnectedComponents();
    void addNodesToConnectedVector(const ColorMap & nodeColorMap,
                                   std::unordered_map<int,std::shared_ptr<MachineGraph>> & colorGraphMap);
    void addEdgesToConnectedVector(const ColorMap & colorMap,
                                   std::unordered_map<int,std::shared_ptr<MachineGraph>> & colorGraphMap);
    void addEdgesToConnectedVector_processEdge(GraphType::EdgeTypePtr edge,
                                               int connectComponentId,
                                               std::unordered_map<int,std::shared_ptr<MachineGraph>> & colorGraphMap);

    std::shared_ptr<int> & getNodeColor(int id, ColorMap & colorMap);

    int fillConnectedComponentsMap(ColorMap & colorMap);
    void fillConnectedComponentsMap_processValves(ColorMap & colorMap,
                                                  GraphType::UniqueEdgeMap & edges,
                                                  int & numConnectedComponents,
                                                  AutoEnumerate & serie);
    void fillConnectedComponentsMap_processEdgeUncutted(std::shared_ptr<int> & sourceColor,
                                                      std::shared_ptr<int> & targetColor,
                                                      int & numConnectedComponents,
                                                      AutoEnumerate & serie);
    void fillConnectedComponentsMap_processEdgeCutted(std::shared_ptr<int> & sourceColor,
                                                      std::shared_ptr<int> & targetColor,
                                                      int & numConnectedComponents,
                                                      AutoEnumerate & serie);

    void processValvesActualPosition(const std::unordered_map<int,std::shared_ptr<MachineGraph>> & graphMap);
    void processValvesActualPosition_processGraph(std::vector<std::shared_ptr<MachineGraph>> & componentsToCheck, std::unordered_set<int> & processedValves);
    int componentNeedProcess(shared_ptr<MachineGraph> component, std::unordered_set<int> & processedValves);
    void processValveSplit(std::shared_ptr<MachineGraph> componet, int valveToProcess, std::vector<std::shared_ptr<MachineGraph>> & componentsToCheck);
    void processValveSplit_removeTubesFromValve(int valveId,
                                                std::shared_ptr<MachineGraph> component,
                                                std::unordered_map<int, GraphType::EdgeTypePtr> & tubesMap);

    void copyGraphStartingFrom(int startNode,
                               std::unordered_set<int> nodesToAvoid,
                               std::shared_ptr<MachineGraph> copyFrom,
                               std::shared_ptr<MachineGraph> copyTo);
};

#endif // MACHINEGRAPH_H
