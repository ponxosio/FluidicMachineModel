#ifndef MACHINEGRAPH_H
#define MACHINEGRAPH_H

#include <memory>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include <commonmodel/functions/function.h>
#include <commonmodel/plugininterface/pluginabstractfactory.h>
#include <graph/Graph.h>
#include <utils/AutoEnumerate.h>
#include <utils/Utils.h>

#include "fluidicmachinemodel/fluidicedge/tubeedge.h"
#include "fluidicmachinemodel/fluidicnode/fluidicmachinenode.h"
#include "fluidicmachinemodel/fluidicnode/pumpnode.h"
#include "fluidicmachinemodel/fluidicnode/valvenode.h"
#include "fluidicmachinemodel/fluidicnode/containernode.h"

#include "fluidicmachinemodel_global.h"

/**
 * @brief The MachineGraph class is a directed graph that represents the layout of a fuidic machine compound of: containers, pumps and valves.
 *
 * The MachineGraph class is a directed graph that represents the layout of a millifuidic machine compound of: containers, pumps and valves.
 * The containers, pumps and valves are the nodes of the graph and the edges represents the physical connections(tubes, grooves..etc)  between
 * them. This class contains method to add, retreive and connect nodes(pumps, valves, containers) and to calculated the connected components of
 * a the layout taking into account the state of the nodes, for example, if a valve is close this methods ignore the edges leaving that node.
 */
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
    GraphType::EdgeVectorPtr getArrivingTubes(int nodeId) const throw(std::invalid_argument);
    GraphType::EdgeVectorPtr getLeavingTubes(int nodeId) const throw(std::invalid_argument);
    GraphType::EdgeTypePtr getTube(int idSource, int idTarget) const throw(std::invalid_argument);
    GraphType::EdgeTypePtr getTubeConnectedToPin(int nodeId, int pinNumber) const throw(std::invalid_argument);

    void cutTube(int idSource, int idTarget) throw(std::invalid_argument);
    void uncutTube(int idSource, int idTarget) throw(std::invalid_argument);
    void cutAllTubesConnectedTo(int idNode) throw(std::invalid_argument);
    void uncutAllTubesConnectedTo(int idNode) throw(std::invalid_argument);
    bool isTubeCutted(int idSource, int idTarget) throw(std::invalid_argument);

    void setTubeDirection(int idSource, int idTarget, TubeEdge::TubeDirection dir) throw(std::invalid_argument);
    void setAllTubesDirectionsConnectedTo(int idNode, TubeEdge::TubeDirection dir) throw(std::invalid_argument);
    TubeEdge::TubeDirection getTubeDirection(int idSource, int idTarget) throw(std::invalid_argument);

    void addNode(std::shared_ptr<FluidicMachineNode> node) throw(std::invalid_argument);
    int emplaceContainer(int numPins, ContainerNode::ContainerType type, double maxVolume);
    int emplacePump(int numPins, PumpNode::PumpType type, std::shared_ptr<Function> pumpFunction) throw(std::invalid_argument);
    int emplaceValve(int numPins, const ValveNode::TruthTable & truthTable,
                  std::shared_ptr<Function> valveRouteFunction) throw(std::invalid_argument);

    bool hasNode(int id) const;
    std::shared_ptr<FluidicMachineNode> getNode(int id) const throw(std::invalid_argument);
    std::shared_ptr<ContainerNode> getContainer(int id) const throw(std::invalid_argument);
    std::shared_ptr<FluidicMachineNode> getFirstNodeOnlyLeavingTubes() const throw(std::invalid_argument);
    std::shared_ptr<PumpNode> getPump(int id) const throw(std::invalid_argument);
    std::shared_ptr<ValveNode> getValve(int id) const throw(std::invalid_argument);

    short int getOpenContainerLiquidId(int idOpenContainer) const throw (std::invalid_argument);

    const std::vector<std::shared_ptr<MachineGraph>> & getConnectedComponents();

    void updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory);

    inline bool isContainer(int id) const {
        return (isOpenContainer(id) || isCloseContainer(id));
    }
    inline bool isOpenContainer(int id) const {
        return (openContainersSet.find(id) != openContainersSet.end());
    }
    inline bool isCloseContainer(int id) const {
        return (closeContainersSet.find(id) != closeContainersSet.end());
    }
    inline bool isPump(int id) const {
        return (pumpsSet.find(id) != pumpsSet.end());
    }
    inline bool isValve(int id) const {
        return (valvesSet.find(id) != valvesSet.end());
    }
    inline bool areConnected(int idSource, int idTarget) const {
        return graphPtr->areConnected(idSource, idTarget);
    }
    inline const unordered_set<int> & getPumpsIdsSet() const {
        return pumpsSet;
    }
    inline const unordered_set<int> & getValvesIdsSet() const {
        return valvesSet;
    }
    inline const unordered_set<int> & getOpenContainersIdsSet() const {
        return openContainersSet;
    }
    inline const unordered_set<int> & getCloseContainersIdsSet() const {
        return closeContainersSet;
    }
    inline const unordered_map<short int, short int> & getOpenContainerLiquidIdMap() const {
        return openContainerLiquidIdMap;
    }

    inline int getNumOpenContainers() const {
        return openContainersSet.size();
    }
    inline GraphType::UniqueEdgeMapPtr getAlledgesMap() const {
        return graphPtr->getAllEdges();
    }
    inline GraphType::NodeMapPtr getAllNodesMap() const {
        return graphPtr->getAllNodes();
    }

    inline std::string toString() const {
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
