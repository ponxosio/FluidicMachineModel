#include "machinegraph.h"

MachineGraph::MachineGraph() {
    graphPtr = std::make_shared<GraphType>();
    connectedComponentsUpdated = true;
}

MachineGraph::MachineGraph(const MachineGraph & mg) :
    sequence(mg.sequence), luquidIdserie(mg.luquidIdserie)
{
    this->graphPtr = std::make_shared<GraphType>(*mg.graphPtr.get());
    this->connectedComponentsUpdated = false;

    for(int id: mg.pumpsSet) {
        this->pumpsSet.insert(id);
    }
    for(int id: mg.valvesSet) {
        this->valvesSet.insert(id);
    }
    for(int id: mg.openContainersSet) {
        this->openContainersSet.insert(id);
    }
    for(int id: mg.closeContainersSet) {
        this->closeContainersSet.insert(id);
    }
    for(const auto & tuple: mg.openContainerLiquidIdMap) {
        this->openContainerLiquidIdMap.insert(std::make_pair(tuple.first, tuple.second));
    }
}

MachineGraph::~MachineGraph() {

}

void MachineGraph::connectNodes(int idSource, int idTarget, int sourcePinToConnect, int targetPinToConnect) throw(std::invalid_argument) {
    if (!graphPtr->areConnected(idSource, idTarget)) {
        GraphType::NodeTypePtr sourceNode = graphPtr->getNode(idSource);
        GraphType::NodeTypePtr targetNode = graphPtr->getNode(idTarget);
        if ( sourceNode != NULL &&
             targetNode != NULL)
        {
            try {
                std::shared_ptr<TubeEdge> edge = std::make_shared<TubeEdge>(idSource, idTarget);

                sourceNode->connectToPin(sourcePinToConnect, edge);
                targetNode->connectToPin(targetPinToConnect, edge);
                graphPtr->addEdge(edge);

                connectedComponentsUpdated = false;
            } catch (std::invalid_argument & e) {
                throw(std::invalid_argument("exception while connecting nodes, message is: " + std::string(e.what())));
            }
        } else {
            throw(std::invalid_argument(std::to_string(idSource) + " or " + std::to_string(idTarget) +
                                        " are not nodes of the graph"));
        }
    } else {
        throw(std::invalid_argument(std::to_string(idSource) + "->" + std::to_string(idTarget) +
                                    " are already connected"));
    }
}

MachineGraph::GraphType::EdgeVectorPtr MachineGraph::getArrivingTubes(int nodeId) throw(std::invalid_argument) {
    auto edges = graphPtr->getArrivingEdges(nodeId);
    if (edges) {
        return edges;
    } else {
        throw(std::invalid_argument("unknow node id " + std::to_string(nodeId)));
    }
}

MachineGraph::GraphType::EdgeVectorPtr MachineGraph::getLeavingTubes(int nodeId) throw(std::invalid_argument) {
    auto edges = graphPtr->getLeavingEdges(nodeId);
    if (edges) {
        return edges;
    } else {
        throw(std::invalid_argument("unknow node id " + std::to_string(nodeId)));
    }
}

MachineGraph::GraphType::EdgeTypePtr MachineGraph::getTube(int idSource, int idTarget) throw(std::invalid_argument) {
    GraphType::EdgeTypePtr edge = graphPtr->getEdge(idSource, idTarget);
    if (edge != NULL) {
        return edge;
    } else {
        throw(std::invalid_argument("MachineGraph::getTube. unknow tube " + std::to_string(idSource) + "->" + std::to_string(idTarget)));
    }
}

MachineGraph::GraphType::EdgeTypePtr MachineGraph::getTubeConnectedToPin(int nodeId, int pinNumber) throw(std::invalid_argument) {
    GraphType::NodeTypePtr nodePtr = graphPtr->getNode(nodeId);
    if (nodePtr != NULL) {
        try {
            std::tuple<int,int> edgeId = nodePtr->getTubeIdConnectedToPin(pinNumber);
            return getTube(std::get<0>(edgeId), std::get<1>(edgeId));
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("exception while getting pin number of node " + std::to_string(nodeId) +
                                        ". messsage " + std::string(e.what())));
        }
    } else {
        throw(std::invalid_argument("unknow nodeId: " + std::to_string(nodeId)));
    }
}

void MachineGraph::cutTube(int idSource, int idTarget) throw(std::invalid_argument) {
    GraphType::EdgeTypePtr edge = graphPtr->getEdge(idSource, idTarget);
    if (edge != nullptr) {
        edge->cutEdge();
        connectedComponentsUpdated = false;
    } else {
        throw(std::invalid_argument("unknow tube " + std::to_string(idSource) + "->" + std::to_string(idTarget)));
    }
}

void MachineGraph::uncutTube(int idSource, int idTarget) throw(std::invalid_argument) {
    GraphType::EdgeTypePtr edge = graphPtr->getEdge(idSource, idTarget);
    if (edge != nullptr) {
        edge->uncutEdge();
        connectedComponentsUpdated = false;
    } else {
        throw(std::invalid_argument("unknow tube " + std::to_string(idSource) + "->" + std::to_string(idTarget)));
    }
}

void MachineGraph::cutAllTubesConnectedTo(int idNode) throw(std::invalid_argument) {
    GraphType::EdgeVectorPtr leavingEdges = graphPtr->getLeavingEdges(idNode);
    GraphType::EdgeVectorPtr arrivingEdges = graphPtr->getArrivingEdges(idNode);

    if (leavingEdges != NULL &&
            arrivingEdges != NULL)
    {
        for (MachineGraph::GraphType::EdgeTypePtr edge : *leavingEdges.get()) {
            edge->cutEdge();
        }
        for (MachineGraph::GraphType::EdgeTypePtr edge : *arrivingEdges.get()) {
            edge->cutEdge();
        }
        connectedComponentsUpdated = false;
    } else {
        throw(std::invalid_argument("unknow node id " + std::to_string(idNode)));
    }
}

void MachineGraph::uncutAllTubesConnectedTo(int idNode) throw(std::invalid_argument) {
    GraphType::EdgeVectorPtr leavingEdges = graphPtr->getLeavingEdges(idNode);
    GraphType::EdgeVectorPtr arrivingEdges = graphPtr->getArrivingEdges(idNode);

    if (leavingEdges != NULL &&
            arrivingEdges != NULL)
    {
        for (MachineGraph::GraphType::EdgeTypePtr edge : *leavingEdges.get()) {
            edge->uncutEdge();
        }
        for (MachineGraph::GraphType::EdgeTypePtr edge : *arrivingEdges.get()) {
            edge->uncutEdge();
        }
        connectedComponentsUpdated = false;
    } else {
        throw(std::invalid_argument("unknow node id " + std::to_string(idNode)));
    }
}

bool MachineGraph::isTubeCutted(int idSource, int idTarget) throw(std::invalid_argument) {
    GraphType::EdgeTypePtr edge = graphPtr->getEdge(idSource, idTarget);
    if (edge != nullptr) {
        return edge->isCutted();
    } else {
        throw(std::invalid_argument("unknow tube " + std::to_string(idSource) + "->" + std::to_string(idTarget)));
    }
}

void MachineGraph::setTubeDirection(int idSource, int idTarget, TubeEdge::TubeDirection dir) throw(std::invalid_argument) {
    GraphType::EdgeTypePtr edge = graphPtr->getEdge(idSource, idTarget);
    if (edge != nullptr) {
        edge->setDirection(dir);
        connectedComponentsUpdated = false;
    } else {
        throw(std::invalid_argument("unknow tube " + std::to_string(idSource) + "->" + std::to_string(idTarget)));
    }
}

void MachineGraph::setAllTubesDirectionsConnectedTo(int idNode, TubeEdge::TubeDirection dir) throw(std::invalid_argument) {
    GraphType::EdgeVectorPtr leavingEdges = graphPtr->getLeavingEdges(idNode);
    GraphType::EdgeVectorPtr arrivingEdges = graphPtr->getArrivingEdges(idNode);

    if (leavingEdges != NULL ||
            arrivingEdges != NULL)
    {
        for (MachineGraph::GraphType::EdgeTypePtr edge : *leavingEdges.get()) {
            edge->setDirection(dir);
        }
        for (MachineGraph::GraphType::EdgeTypePtr edge : *arrivingEdges.get()) {
            edge->setDirection(dir);
        }
        connectedComponentsUpdated = false;
    } else {
        throw(std::invalid_argument("unknow node id " + std::to_string(idNode)));
    }
}

TubeEdge::TubeDirection MachineGraph::getTubeDirection(int idSource, int idTarget) throw(std::invalid_argument) {
    GraphType::EdgeTypePtr edge = graphPtr->getEdge(idSource, idTarget);
    if (edge != nullptr) {
        return edge->getDirection();
    } else {
        throw(std::invalid_argument("unknow tube " + std::to_string(idSource) + "->" + std::to_string(idTarget)));
    }
}

void MachineGraph::addNode(std::shared_ptr<FluidicMachineNode> node) throw(std::invalid_argument) {
    if(graphPtr->getNode(node->getContainerId()) == NULL) {
        if (std::shared_ptr<ContainerNode> container = std::dynamic_pointer_cast<ContainerNode>(node)) {
           graphPtr->addNode(node);
           if (container->getContainerType() == ContainerNode::open) {
               int liquidId = luquidIdserie.getNextValue();
               openContainerLiquidIdMap.insert(std::make_pair(node->getContainerId(), liquidId));

               openContainersSet.insert(node->getContainerId());
           } else {
               closeContainersSet.insert(node->getContainerId());
           }
        } else if (std::dynamic_pointer_cast<PumpNode>(node)) {
            graphPtr->addNode(node);
            pumpsSet.insert(node->getContainerId());
        } else if (std::dynamic_pointer_cast<ValveNode>(node)) {
            graphPtr->addNode(node);
            valvesSet.insert(node->getContainerId());
        } else {
            throw(std::invalid_argument("node of unknow subtype, not pump valve or container"));
        }
        connectedComponentsUpdated = false;
    } else {
        throw(std::invalid_argument("repeated ID:" + std::to_string(node->getContainerId()) + " is already being use as an id by other node"));
    }
}

int MachineGraph::emplaceContainer(int numPins, ContainerNode::ContainerType type, double maxVolume) {
    int id = sequence.getNextValue();
    std::shared_ptr<FluidicMachineNode> node = std::make_shared<ContainerNode>(id, numPins, type, maxVolume);
    graphPtr->addNode(node);

    if (type == ContainerNode::open) {
        int liquidId = luquidIdserie.getNextValue();
        openContainerLiquidIdMap.insert(std::make_pair(id, liquidId));

        openContainersSet.insert(id);
    } else {
        closeContainersSet.insert(id);
    }
    connectedComponentsUpdated = false;
    return id;
}

int MachineGraph::emplacePump(int numPins, PumpNode::PumpType type, std::shared_ptr<Function> pumpFunction) throw(std::invalid_argument) {
    try {
        int id = sequence.getNextValue();
        std::shared_ptr<FluidicMachineNode> node = std::make_shared<PumpNode>(id, numPins, type, pumpFunction);

        graphPtr->addNode(node);
        pumpsSet.insert(id);
        connectedComponentsUpdated = false;

        return id;
    } catch(std::invalid_argument & e) {
        throw(std::invalid_argument("imposible to addPump: error message " + std::string(e.what())));
    }
}

int MachineGraph::emplaceValve(int numPins, const ValveNode::TruthTable & truthTable,
              std::shared_ptr<Function> valveRouteFunction) throw(std::invalid_argument)
{
    try {
        int id = sequence.getNextValue();
        std::shared_ptr<FluidicMachineNode> node = std::make_shared<ValveNode>(id, numPins, truthTable, valveRouteFunction);

        graphPtr->addNode(node);
        valvesSet.insert(id);
        connectedComponentsUpdated = false;

        return id;
    } catch(std::invalid_argument & e) {
        throw(std::invalid_argument("imposible to addValve: error message " + std::string(e.what())));
    }
}

bool MachineGraph::hasNode(int id) {
    return (graphPtr->getNode(id) != NULL);
}

std::shared_ptr<FluidicMachineNode> MachineGraph::getNode(int id) throw(std::invalid_argument) {
    GraphType::NodeTypePtr node = graphPtr->getNode(id);
    if (node != NULL) {
        return node;
    } else {
        throw(std::invalid_argument("unknow node id " + std::to_string(id)));
    }
}

std::shared_ptr<ContainerNode> MachineGraph::getContainer(int id) throw(std::invalid_argument) {
    if (isContainer(id)) {
        GraphType::NodeTypePtr node = graphPtr->getNode(id);
        if (node != NULL) {
            return std::dynamic_pointer_cast<ContainerNode>(node);
        } else {
            throw(std::invalid_argument("unknow node id " + std::to_string(id)));
        }
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not a container"));
    }
}

std::shared_ptr<FluidicMachineNode> MachineGraph::getFirstNodeOnlyLeavingTubes() throw(std::invalid_argument) {
    std::shared_ptr<FluidicMachineNode> finded = NULL;
    GraphType::NodeMapPtr nodeMap = graphPtr->getAllNodes();

    if (!nodeMap->empty()) {
        if (nodeMap->size() == 1) {
            finded = nodeMap->begin()->second;
        } else {
            for(auto nodeTuple: *nodeMap.get()) {
                GraphType::EdgeVectorPtr leaving = graphPtr->getLeavingEdges(nodeTuple.first);
                GraphType::EdgeVectorPtr arriving = graphPtr->getArrivingEdges(nodeTuple.first);

                if (leaving->size() != 0 &&
                        arriving->size() == 0)
                {
                    finded = nodeTuple.second;
                }
            }
        }

    } else {
        throw(std::invalid_argument("The graph does not contains nodes"));
    }
    return finded;
}

std::shared_ptr<PumpNode> MachineGraph::getPump(int id) throw(std::invalid_argument) {
    if (isPump(id)) {
        GraphType::NodeTypePtr node = graphPtr->getNode(id);
        if (node != NULL) {
            return std::dynamic_pointer_cast<PumpNode>(node);
        } else {
            throw(std::invalid_argument("unknow node id " + std::to_string(id)));
        }
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not a pump"));
    }
}

std::shared_ptr<ValveNode> MachineGraph::getValve(int id) throw(std::invalid_argument) {
    if (isValve(id)) {
        GraphType::NodeTypePtr node = graphPtr->getNode(id);
        if (node != NULL) {
            return std::dynamic_pointer_cast<ValveNode>(node);
        } else {
            throw(std::invalid_argument("unknow node id " + std::to_string(id)));
        }
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not a valve"));
    }
}

short int MachineGraph::getOpenContainerLiquidId(int idOpenContainer) throw (std::invalid_argument) {
    auto finded = openContainerLiquidIdMap.find(idOpenContainer);
    if (finded != openContainerLiquidIdMap.end()) {
        return finded->second;
    } else {
        throw(std::invalid_argument(std::to_string(idOpenContainer) + " is not an open container"));
    }
}

void MachineGraph::updatePluginFactory(std::shared_ptr<PluginAbstractFactory> factory) {
    GraphType::NodeMapPtr nodes = graphPtr->getAllNodes();
    for (auto pair: *nodes.get()) {
        pair.second->setFactory(factory);
    }
}

const std::vector<std::shared_ptr<MachineGraph>> & MachineGraph::getConnectedComponents() {
    if(!connectedComponentsUpdated) {
        calculateConnectedComponents();
        connectedComponentsUpdated = true;
    }
    return connectedComponentsVector;
}

void MachineGraph::calculateConnectedComponents() {
    ColorMap colorMap;
    int maxConnectedComponents = fillConnectedComponentsMap(colorMap);

    connectedComponentsVector.clear();
    connectedComponentsVector.reserve(maxConnectedComponents);

    std::unordered_map<int,std::shared_ptr<MachineGraph>> colorGraphMap;

    addNodesToConnectedVector(colorMap, colorGraphMap);
    addEdgesToConnectedVector(colorMap, colorGraphMap);

    processValvesActualPosition(colorGraphMap);
}

/**
 * Search all disjoint subgraphs, the idea is to color connect nodes with the same color:
 *
 * for all edges->
 * 	*) if both connected nodes has no color: set the same random color to both;
 * 	*) if only one has a color: set the empty node's color so they be the same.
 * 	*) if both has one color and those colors are different: update all the nodes with
 * 		the bigger color to match the smaller one. (merge)
 */
int MachineGraph::fillConnectedComponentsMap(ColorMap & colorMap) {
    AutoEnumerate serie;
    int numConnectedComponents = 0;
    GraphType::UniqueEdgeMapPtr edges = graphPtr->getAllEdges();

    for (auto pair: *edges.get()) {
        GraphType::EdgeTypePtr edge = pair.second;
        std::shared_ptr<int> & sourceColor = getNodeColor(edge->getIdSource(), colorMap);
        std::shared_ptr<int> & targetColor = getNodeColor(edge->getIdTarget(), colorMap);

        if (!edge->isCutted()) {
            fillConnectedComponentsMap_processEdgeUncutted(sourceColor, targetColor, numConnectedComponents, serie);
        } else {
            fillConnectedComponentsMap_processEdgeCutted(sourceColor, targetColor, numConnectedComponents, serie);
        }
    }
    return numConnectedComponents;
}

void MachineGraph::fillConnectedComponentsMap_processEdgeUncutted(std::shared_ptr<int> & sourceColor,
                                                                std::shared_ptr<int> & targetColor,
                                                                int & numConnectedComponents,
                                                                AutoEnumerate & serie)
{
    if ((*sourceColor.get() == -1) &&
            (*targetColor.get() == -1))
    {
        int nextColor = serie.getNextValue();
        *sourceColor.get() = nextColor;
        targetColor = sourceColor;

        numConnectedComponents++;
    }
    else if ((*sourceColor.get() != -1) &&
             (*targetColor.get() == -1))
    {
        targetColor = sourceColor;
    }
    else if ((*sourceColor.get() == -1) &&
             (*targetColor.get() != -1))
    {
        sourceColor = targetColor;
    }
    else if (*sourceColor.get() != *targetColor.get()){
        int minColor = std::min(*sourceColor.get(), *targetColor.get());
        *sourceColor.get() = minColor;
        *targetColor.get() = minColor;

        numConnectedComponents--;
    }
}

void MachineGraph::fillConnectedComponentsMap_processEdgeCutted(std::shared_ptr<int> & sourceColor,
                                                                std::shared_ptr<int> & targetColor,
                                                                int & numConnectedComponents,
                                                                AutoEnumerate & serie)
{
    if ((*sourceColor.get() == -1)) {
        int nextColor = serie.getNextValue();
        *sourceColor.get() = nextColor;

        numConnectedComponents++;
    }
    if ((*targetColor.get() == -1)) {
        int nextColor = serie.getNextValue();
        *targetColor.get() = nextColor;

        numConnectedComponents++;
    }
}

std::shared_ptr<int> & MachineGraph::getNodeColor(int id, ColorMap & colorMap) {

    auto it = colorMap.find(id);
    if (it != colorMap.end()) {
       return colorMap.at(id);
    } else {
        std::shared_ptr<int> unknowColor = std::make_shared<int>(-1);
        colorMap.insert(std::make_pair(id, unknowColor));
        return colorMap.at(id);
    }
}

void MachineGraph::addNodesToConnectedVector(const ColorMap & colorMap,
                                             std::unordered_map<int,std::shared_ptr<MachineGraph>> & colorGraphMap)
{
    GraphType::NodeMapPtr allNodes = graphPtr->getAllNodes();
    for (auto it = allNodes->begin(); it != allNodes->end(); ++it) {
        int nodeId = it->first;
        std::shared_ptr<int> connectComponentPtr = colorMap.find(nodeId)->second;
        int connectComponentId = *connectComponentPtr.get();

        std::shared_ptr<MachineGraph> tempGraph = nullptr;
        auto connectedComponent = colorGraphMap.find(connectComponentId);
        if (connectedComponent == colorGraphMap.end()) {
            tempGraph = std::make_shared<MachineGraph>();
            colorGraphMap.insert(std::make_pair(connectComponentId, tempGraph));
        } else {
            tempGraph = connectedComponent->second;
        }
        tempGraph->addNode(graphPtr->getNode(nodeId));
    }
}

void MachineGraph::addEdgesToConnectedVector(const ColorMap & colorMap,
                                             std::unordered_map<int,std::shared_ptr<MachineGraph>> & colorGraphMap)
{
    GraphType::UniqueEdgeMapPtr allEdges = graphPtr->getAllEdges();
    for(auto it = allEdges->begin(); it != allEdges->end(); ++it) {
        GraphType::EdgeTypePtr edge = it->second;
        int edgeId = edge->getIdSource();

        std::shared_ptr<int> connectComponentPtr = colorMap.find(edgeId)->second;
        int connectComponentId = *connectComponentPtr.get();
        addEdgesToConnectedVector_processEdge(edge, connectComponentId, colorGraphMap);
    }
}

void MachineGraph::addEdgesToConnectedVector_processEdge(GraphType::EdgeTypePtr edge,
                                                         int connectComponentId,
                                                         std::unordered_map<int,std::shared_ptr<MachineGraph>> & colorGraphMap)
{
    std::shared_ptr<MachineGraph> tempGraph = nullptr;
    auto connectedComponent = colorGraphMap.find(connectComponentId);
    if (connectedComponent == colorGraphMap.end()) {
        tempGraph = std::make_shared<MachineGraph>();
        colorGraphMap.insert(std::make_pair(connectComponentId, tempGraph));
    } else {
        tempGraph = connectedComponent->second;
    }
    tempGraph->graphPtr->addEdge(edge);
}

void MachineGraph::processValvesActualPosition(const std::unordered_map<int,std::shared_ptr<MachineGraph>> & graphMap) {
    for(auto pair : graphMap) {
        std::unordered_set<int> processedValves;
        std::vector<std::shared_ptr<MachineGraph>> componentsToCheck;
        componentsToCheck.push_back(pair.second);

        processValvesActualPosition_processGraph(componentsToCheck, processedValves);
    }
}

bool MachineGraph::valveNeedProcess(int valveId, shared_ptr<MachineGraph> component) {
    std::shared_ptr<ValveNode> valvePtr = component->getValve(valveId);
    const std::vector<std::unordered_set<int>> & pinsConnected = valvePtr->getConnectedPins(valvePtr->getActualPosition());
    bool needProcess = (pinsConnected.size() > 1);
    return needProcess;
}

int MachineGraph::componentNeedProcess(shared_ptr<MachineGraph> component, std::unordered_set<int> & processedValves) {
    int valveTocheck = -1;
    for (auto it = component->valvesSet.begin(); valveTocheck == -1 && it != component->valvesSet.end(); ++it) {
        int actualValve = *it;
        if (processedValves.find(actualValve) == processedValves.end()) {
            if (valveNeedProcess(actualValve, component)) {
                valveTocheck = actualValve;
            }
            processedValves.insert(actualValve);
        }
    }
    return valveTocheck;
}

void MachineGraph::processValvesActualPosition_processGraph(std::vector<std::shared_ptr<MachineGraph>> & componentsToCheck, std::unordered_set<int> & processedValves) {
    while(!componentsToCheck.empty()) {
        std::shared_ptr<MachineGraph> actualComponent = componentsToCheck.back();
        componentsToCheck.pop_back();

        int valveToProcess = componentNeedProcess(actualComponent, processedValves);
        if (valveToProcess != -1) {
            processValveSplit(actualComponent, valveToProcess, componentsToCheck);
        } else {
            connectedComponentsVector.push_back(actualComponent);
        }
    }
}

void MachineGraph::processValveSplit(std::shared_ptr<MachineGraph> componet,
                                     int valveToProcess,
                                     std::vector<std::shared_ptr<MachineGraph>> & componentsToCheck)
{
    std::shared_ptr<ValveNode> valvePtr = componet->getValve(valveToProcess);

    const std::vector<std::unordered_set<int>> & connectedPinsVector = valvePtr->getConnectedPins(valvePtr->getActualPosition());
    for(const std::unordered_set<int> & connectedPins: connectedPinsVector) {
        std::shared_ptr<MachineGraph> newComponent = std::make_shared<MachineGraph>();
        newComponent->addNode(valvePtr);

        GraphType::EdgeVectorPtr leaving = componet->getLeavingTubes(valveToProcess);
        for(GraphType::EdgeTypePtr tube: *leaving.get()) {
            if (tube->isCutted()) {
                newComponent->graphPtr->addEdge(tube);
            }
        }
        GraphType::EdgeVectorPtr arriving = componet->getArrivingTubes(valveToProcess);
        for(GraphType::EdgeTypePtr tube: *arriving.get()) {
            if (tube->isCutted()) {
                newComponent->graphPtr->addEdge(tube);
            }
        }

        for(int pin: connectedPins) {
            std::tuple<int,int> tubeTuple = valvePtr->getTubeIdConnectedToPin(pin);
            GraphType::EdgeTypePtr tube = componet->graphPtr->getEdge(std::get<0>(tubeTuple), std::get<1>(tubeTuple));

            if (tube != NULL) {
                int nodeStartCopy = -1;
                if (tube->getIdSource() == valveToProcess) {
                    nodeStartCopy = tube->getIdTarget();
                } else {
                    nodeStartCopy = tube->getIdSource();
                }
                std::unordered_set<int> nodesToAvoid;
                nodesToAvoid.insert(valveToProcess);
                copyGraphStartingFrom(nodeStartCopy, nodesToAvoid, componet, newComponent);
            }
        }
        componentsToCheck.push_back(newComponent);
    }
}

void MachineGraph::copyGraphStartingFrom(int startNode,
                                         std::unordered_set<int> nodesToAvoid,
                                         std::shared_ptr<MachineGraph> copyFrom,
                                         std::shared_ptr<MachineGraph> copyTo)
{
    std::vector<int> nodes2Check;
    nodes2Check.push_back(startNode);

    while(!nodes2Check.empty()) {
        int node2process = nodes2Check.back();
        nodes2Check.pop_back();

        if (copyTo->graphPtr->getNode(node2process) == NULL) {
            GraphType::NodeTypePtr nodePtr = copyFrom->getNode(node2process);
            copyTo->addNode(nodePtr);
        }

        auto finded = nodesToAvoid.find(node2process);
        if (finded == nodesToAvoid.end()) {
            nodesToAvoid.insert(node2process);

            GraphType::EdgeVectorPtr arriving = copyFrom->getArrivingTubes(node2process);
            for(GraphType::EdgeTypePtr edge: *arriving.get()) {
                if (!copyTo->areConnected(edge->getIdSource(), edge->getIdTarget())) {
                    if (!edge->isCutted()) {
                        int nodeId = edge->getIdSource();
                        if (copyTo->graphPtr->getNode(nodeId) == NULL) {
                            GraphType::NodeTypePtr nodePtr = copyFrom->getNode(nodeId);
                            copyTo->addNode(nodePtr);
                        }
                        nodes2Check.push_back(nodeId);
                    }
                    copyTo->graphPtr->addEdge(edge);
                }
            }

            GraphType::EdgeVectorPtr leaving = copyFrom->getLeavingTubes(node2process);
            for(GraphType::EdgeTypePtr edge: *leaving.get()) {
                if (!copyTo->areConnected(edge->getIdSource(), edge->getIdTarget())) {
                    if (!edge->isCutted()) {
                        int nodeId = edge->getIdTarget();
                        if (copyTo->graphPtr->getNode(nodeId) == NULL) {
                            GraphType::NodeTypePtr nodePtr = copyFrom->getNode(nodeId);
                            copyTo->addNode(nodePtr);

                            nodes2Check.push_back(nodeId);
                        }
                    }
                    copyTo->graphPtr->addEdge(edge);
                }
            }

        }
    }
}























