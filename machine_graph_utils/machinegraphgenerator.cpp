#include "machinegraphgenerator.h"

MachineGraphGenerator::MachineGraphGenerator(GraphPtr graph,
                      const std::vector<int> * pumps,
                      const std::vector<int> * valves) :
    statesSequenceVector(pumps->size() + valves->size())
{
    this->graph = graph;
    this->pumps = pumps;
    this->valves = valves;
    this->maxSequenceIndex = 0;
    this->actualSequenceIndex = 0;

    std::vector<int> vZero = {0};
    statesSequenceVector.push_back(new Sequence(-1,1,&vZero));
    for(int i = 1; i < pumps->size(); i++) {
        statesSequenceVector.push_back(new Sequence(-1,1));
    }

    for(int valveId : *valves) {
        std::shared_ptr<ValveNode> valvePtr = std::dynamic_pointer_cast<ValveNode>(graph->getNode(valveId));
        statesSequenceVector.push_back(new Sequence(valvePtr->getValvePossibleValues()));
    }
}

MachineGraphGenerator::~MachineGraphGenerator() {
    //remove the Sequence variables
    for(Sequence* seq: statesSequence) {
        delete seq;
    }
}

bool MachineGraphGenerator::hasNext() {
    return (maxSequenceIndex >= statesSequenceVector.size());
}

std::vector<MachineGraphGenerator::GraphPtr> MachineGraphGenerator::next() throw(std::invalid_argument) {
    if (hasNext()) {
        std::vector<short int> nextState = getNextStateFromSequence();
        advanceSequence();
        GraphPtr nextGraph = createGraphForState(nextState);

        if (isValidGraph(nextGraph)) {
            return calculateSubgraphs(nextGraph);
        } else {
            return next();
        }
    } else {
        throw(std::invalid_argument("ended sequence"));
    }
}

std::vector<short int> MachineGraphGenerator::getNextStateFromSequence() {
    std::vector<short int> sequenceState;
    for (Sequence* seq: sequenceState) {
        sequenceState.push_back((short int) seq->getActualValue());
    }
    return sequenceState;
}

void MachineGraphGenerator::advanceSequence() {
    int localIndex = 0;
    bool hasAdvanced = false;

    while(!hasAdvanced &&
          (localIndex < statesSequenceVector.size()))
    {
        Sequence* actualSeq = statesSequenceVector[localIndex];
        if(actualSeq->hasEnded()) {
            actualSeq->reset();
            localIndex++;
        } else {
            actualSeq->nextValue();
            hasAdvanced = true;
        }
    }
    maxSequenceIndex = std::max(maxSequenceIndex, localIndex);
}

MachineGraphGenerator::GraphPtr MachineGraphGenerator::createGraphForState(const std::vector<short int> & estates) {

}
