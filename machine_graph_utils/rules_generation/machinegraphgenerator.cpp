#include "machinegraphgenerator.h"

MachineGraphGenerator::MachineGraphGenerator(std::shared_ptr<MachineGraph> graph)
{
    this->graph_original = graph;
    this->maxSequenceIndex = 0;
    this->pumpsMaxIndex = graph->getPumpsIdsSet().size();

    for(auto it = graph->getPumpsIdsSet().begin(); it != graph->getPumpsIdsSet().end(); ++it) {
        int pumpId = *it;
        std::shared_ptr<PumpNode> pumpPtr = graph->getPump(pumpId);
        if (pumpPtr->getType() == PumpNode::bidirectional) {
            statesSequenceVector.push_back(std::make_tuple(pumpId, new Sequence(-1,1)));
        } else {
            statesSequenceVector.push_back(std::make_tuple(pumpId, new Sequence(0,1)));
        }
    }

    for(int valveId : graph->getValvesIdsSet()) {
        std::shared_ptr<ValveNode> valvePtr = graph->getValve(valveId);
        statesSequenceVector.push_back(std::make_tuple(valveId , new Sequence(valvePtr->getValvePossibleValues())));
    }

    actualGraphGenerated = nullptr;
}

MachineGraphGenerator::~MachineGraphGenerator() {
    //remove the Sequence variables
    for(auto tuple: statesSequenceVector) {
        Sequence* seq = std::get<1>(tuple);
        delete seq;
    }
}

std::string MachineGraphGenerator::getStateStr() {
    stringstream stream;

    for (int i = 0; i < actualStateGeneratedVector.size(); i++) {
        int seq = std::get<1>(actualStateGeneratedVector[i]);
        stream << seq << ";";
    }
    return stream.str();
}

bool MachineGraphGenerator::calculateNext() throw(std::invalid_argument) {
    bool advanced = false;
    if (maxSequenceIndex < statesSequenceVector.size()) {
        std::vector<short int> nextState(getNextStateFromSequence());
        advanceSequence();

        if (isValidSequence(nextState)) {
            std::shared_ptr<MachineGraph> nextGraph = createGraphForState(nextState);
            if (isValidGraph(nextGraph)) {
                actualGraphGenerated = nextGraph;
                fillActualStateGeneratedVector(nextState);
                advanced = true;
            }
        }

        if (!advanced) {
            advanced = calculateNext();
        }
    }
    return advanced;
}

void MachineGraphGenerator::fillActualStateGeneratedVector(const std::vector<short int> & actualState) {
    actualStateGeneratedVector.clear();
    actualStateGeneratedVector.reserve(actualState.size());

    for(int i = 0; i < actualState.size(); i++) {
        int id = std::get<0>(statesSequenceVector[i]);
        int state = actualState[i];
        actualStateGeneratedVector.insert(actualStateGeneratedVector.begin() + i, std::make_tuple(id, state));
    }
}

std::vector<short int> MachineGraphGenerator::getNextStateFromSequence() {
    std::vector<short int> sequenceState(statesSequenceVector.size());

    for (int i = 0; i < statesSequenceVector.size(); i++) {
        Sequence* seq = std::get<1>(statesSequenceVector[i]);
        sequenceState[i] = (short int) (seq->getActualValue());
    }
    return sequenceState;
}

void MachineGraphGenerator::advanceSequence() {
    int localIndex = 0;
    bool hasAdvanced = false;

    while(!hasAdvanced &&
          (localIndex < statesSequenceVector.size()))
    {
        Sequence* actualSeq = std::get<1>(statesSequenceVector[localIndex]);
        if(actualSeq->advanceSequence()) {
            hasAdvanced = true;
        } else {
            actualSeq->reset();
            localIndex++;
        }
    }
    maxSequenceIndex = std::max(maxSequenceIndex, localIndex);
}

bool MachineGraphGenerator::isValidSequence(const std::vector<short int> & sequence) {
    int pumpsSum = std::accumulate(sequence.begin(), sequence.begin() + pumpsMaxIndex, (int)0, AbsuluteValueSum());
    int valvesSum = std::accumulate(sequence.begin() + pumpsMaxIndex, sequence.end(), (int)0, AbsuluteValueSum());

    return ((pumpsSum != 0) && (valvesSum != 0));
}

std::shared_ptr<MachineGraph> MachineGraphGenerator::createGraphForState(const std::vector<short int> & states) {
    bool congruent = true;

    std::shared_ptr<MachineGraph> graphCopy = std::make_shared<MachineGraph>(*graph_original.get());
    for(int i = (states.size()-1); congruent && (i >= 0); i--) {
        if (i < pumpsMaxIndex) {
            int pumpId = std::get<0>(statesSequenceVector[i]);
            congruent = setPump(pumpId, states[i], graphCopy);
        } else {
            int valveId = std::get<0>(statesSequenceVector[i]);
            setValve(valveId, states[i], graphCopy);
        }
    }
    return (congruent ? graphCopy : nullptr);
}

bool MachineGraphGenerator::setPump(int pumpId, short int state, std::shared_ptr<MachineGraph> graph) {
    bool congruent = true;
    if (state == 0) {
        graph->cutAllTubesConnectedTo(pumpId);
    } else if (state == 1) {
        congruent = setTubesDirectionFromPump(pumpId, TubeEdge::regular, graph);
    } else if (state == -1) {
        congruent = setTubesDirectionFromPump(pumpId, TubeEdge::inverted, graph);
    }
    return congruent;
}

bool MachineGraphGenerator::setTubesDirectionFromPump(int pumpId, TubeEdge::TubeDirection dir, std::shared_ptr<MachineGraph> graph) throw(std::invalid_argument) {
    std::unordered_set<int> nodesChecked;
    bool congruent = true;

    int lastNode = pumpId;
    std::vector<int> nodesToCheck;
    nodesToCheck.push_back(pumpId);
    do {
        int actualNode = nodesToCheck.back();
        nodesToCheck.pop_back();
        nodesChecked.insert(actualNode);

        if (graph->isValve(actualNode) && MachineGraph::valveNeedProcess(actualNode, graph)) {
            congruent = processMultipathValve(actualNode, lastNode, dir, graph, nodesChecked, nodesToCheck);
        } else {
            congruent = processTubeVector(*graph->getLeavingTubes(actualNode).get(), dir, actualNode, graph, nodesChecked, nodesToCheck);
            if (congruent) {
                congruent = processTubeVector(*graph->getArrivingTubes(actualNode).get(), dir, actualNode, graph, nodesChecked, nodesToCheck);
            }
        }
        lastNode = actualNode;
    } while(congruent && !nodesToCheck.empty());

    return congruent;
}

bool MachineGraphGenerator::processTubeVector(const MachineGraph::GraphType::EdgeVector & tubesToCheck,
                                              TubeEdge::TubeDirection dir,
                                              int nodeProcesing,
                                              std::shared_ptr<MachineGraph> graph,
                                              const std::unordered_set<int> & nodesChecked,
                                              std::vector<int> & nodesToCheck)
{
    bool congruent = true;
    for(auto it = tubesToCheck.begin(); congruent && it != tubesToCheck.end(); ++it) {
        MachineGraph::GraphType::EdgeTypePtr tube = *it;

        if (tube->getDirection() == TubeEdge::unknow ||
            tube->getDirection() == dir)
        {
            tube->setDirection(dir);

            int nextId = (nodeProcesing == tube->getIdSource() ? tube->getIdTarget() : tube->getIdSource());
            if (!graph->isOpenContainer(nextId) &&
                nodesChecked.find(nextId) == nodesChecked.end())
            {
                nodesToCheck.push_back(nextId);
            }
        } else {
            congruent = false;
        }
    }
    return congruent;
}

bool MachineGraphGenerator::processMultipathValve(int valveNode,
                                                  int enteringNode,
                                                  TubeEdge::TubeDirection dir,
                                                  std::shared_ptr<MachineGraph> graph,
                                                  const std::unordered_set<int> & nodesChecked,
                                                  std::vector<int> & nodesToCheck)
{
    bool congruent = true;
    std::shared_ptr<ValveNode> valvePtr = graph->getValve(valveNode);

    bool finded = false;
    const std::vector<std::unordered_set<int>> & connectedPinsVector = valvePtr->getConnectedPins(valvePtr->getActualPosition());
    for (auto it = connectedPinsVector.begin(); !finded && it != connectedPinsVector.end(); ++it) {
        const std::unordered_set<int> & pins = *it;

        MachineGraph::GraphType::EdgeVector tubesToProcess;
        tubesToProcess.reserve(pins.size());

        int i = 0;
        for(int pin: pins) {
            std::tuple<int, int> tubeId = valvePtr->getTubeIdConnectedToPin(pin);
            int idSource = std::get<0>(tubeId);
            int idTarget = std::get<1>(tubeId);

            if ((idSource == valveNode && idTarget == enteringNode) ||
                (idSource == enteringNode && idTarget == valveNode))
            {
                finded = true;
            }
            tubesToProcess.insert(tubesToProcess.begin() + i, graph->getTube(idSource, idTarget));
            i++;
        }

        if (finded) {
            congruent = processTubeVector(tubesToProcess, dir, valveNode, graph, nodesChecked, nodesToCheck);
        }
    }
    return congruent;
}

void MachineGraphGenerator::setValve(int valveId, short int state, std::shared_ptr<MachineGraph> graph) {
    std::shared_ptr<ValveNode> valveNode = graph->getValve(valveId);
    graph->cutAllTubesConnectedTo(valveId);
    try {
        const std::vector<std::unordered_set<int>> & connectedPins = valveNode->getConnectedPins(state);
        for (std::unordered_set<int> connectedSet :  connectedPins) {
            for (int pinId: connectedSet) {
                std::shared_ptr<TubeEdge> tube = graph->getTubeConnectedToPin(valveId, pinId);
                tube->uncutEdge();
            }
        }
        valveNode->setActualPosition(state);
    } catch(std::invalid_argument & e) {
        throw(std::invalid_argument("exception while setting valve state. Error " + std::string(e.what())));
    }
}

bool MachineGraphGenerator::isValidGraph(const std::shared_ptr<MachineGraph> & graph) {
    bool valid = (graph != nullptr);
    if (valid) {
        valid = false;
        const std::vector<std::shared_ptr<MachineGraph>> & connectedComponents = graph->getConnectedComponents();
        for(auto it = connectedComponents.begin(); !valid && (it != connectedComponents.end()); ++it) {
            valid = isValidConnectedComponent(*it);
        }
    }
    return valid;
}

bool MachineGraphGenerator::canOpenContainersConnect(const std::shared_ptr<MachineGraph> & graph_original) {
    int numValidOpenContainers = 0;

    const std::unordered_set<int> & openContainers = graph_original->getOpenContainersIdsSet();
    for(auto it = openContainers.begin(); numValidOpenContainers < 2 && it != openContainers.end(); ++it) {
        bool hasOpenTube = false;
        const MachineGraph::GraphType::EdgeVectorPtr & leaving = graph_original->getLeavingTubes(*it);

        auto leavingIt = leaving->begin();
        while(!hasOpenTube &&
              leavingIt != leaving->end())
        {
            hasOpenTube = (*leavingIt)->isCutted();
            ++leavingIt;
        }

        const MachineGraph::GraphType::EdgeVectorPtr & arriving = graph_original->getArrivingTubes(*it);

        auto arrivingIt = arriving->begin();
        while(!hasOpenTube &&
              arrivingIt != arriving->end())
        {
            hasOpenTube = (*arrivingIt)->isCutted();
            ++arrivingIt;
        }

        if (hasOpenTube) {
            numValidOpenContainers++;
        }
    }
    return (numValidOpenContainers >= 2);
}

bool MachineGraphGenerator::isValidConnectedComponent(const std::shared_ptr<MachineGraph> & graph) {
    bool valid = (graph->getOpenContainersIdsSet().size() >= 2);
    if (valid) {
        for (auto it = graph->getCloseContainersIdsSet().begin(); valid && it != graph->getCloseContainersIdsSet().end(); ++it) {
            int numEdgesLeaving = 0;
            int numEdgesArring = 0;

            countLeavingArringEdgesVector(graph->getArrivingTubes(*it), true, numEdgesArring, numEdgesLeaving);
            countLeavingArringEdgesVector(graph->getLeavingTubes(*it), false, numEdgesArring, numEdgesLeaving);

            valid = ((numEdgesArring > 0) && (numEdgesArring > 0));
        }
    }
    return valid;
}

void MachineGraphGenerator::countLeavingArringEdgesVector(const MachineGraph::GraphType::EdgeVectorPtr & tubesToCheck,
                                   bool arriving,
                                   int & numArriving,
                                   int & numLeaving)
{
    for(MachineGraph::GraphType::EdgeTypePtr tube: *tubesToCheck.get()) {
        if (!tube->isCutted()) {
            if (arriving) {
                if (tube->getDirection() == TubeEdge::regular) {
                    numArriving++;
                } else if (tube->getDirection() == TubeEdge::inverted) {
                    numLeaving++;
                }
            } else {
                if (tube->getDirection() == TubeEdge::regular) {
                    numLeaving++;
                } else if (tube->getDirection() == TubeEdge::inverted) {
                    numArriving++;
                }
            }
        }
    }
}



























