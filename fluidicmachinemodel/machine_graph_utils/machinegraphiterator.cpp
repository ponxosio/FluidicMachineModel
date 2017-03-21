#include "machinegraphiterator.h"

MachineGraphIterator::MachineGraphIterator(int startNode, std::shared_ptr<MachineGraph> graph) {
    this->actualNode = -1;
    this->graphPtr = graph;

    nodes2process.push_back(startNode);
    nodesToAvoid.insert(startNode);
}

MachineGraphIterator::MachineGraphIterator(int startNode, std::shared_ptr<MachineGraph> graph, const std::unordered_set<int> & nodesToAvoid) :
    nodesToAvoid(nodesToAvoid)
{
    this->actualNode = -1;
    this->graphPtr = graph;

    this->nodes2process.push_back(startNode);
    this->nodesToAvoid.insert(startNode);
}

MachineGraphIterator::~MachineGraphIterator() {

}

bool MachineGraphIterator::advance() {
    bool advanced = false;
    if (!nodes2process.empty()) {
        advanced = true;

        actualNode = nodes2process.back();
        nodes2process.pop_back();

        const MachineGraph::GraphType::EdgeVectorPtr & leaving = graphPtr->getLeavingTubes(actualNode);
        for(const MachineGraph::GraphType::EdgeTypePtr & actualTube: *leaving.get()) {
            int nextNode = actualTube->getIdTarget();

            if (nodesToAvoid.find(nextNode) == nodesToAvoid.end()) {
                nodesToAvoid.insert(nextNode);
                nodes2process.push_back(nextNode);
            }
        }

        const MachineGraph::GraphType::EdgeVectorPtr & arriving = graphPtr->getArrivingTubes(actualNode);
        for(const MachineGraph::GraphType::EdgeTypePtr & actualTube: *arriving.get()) {
            int nextNode = actualTube->getIdSource();

            if (nodesToAvoid.find(nextNode) == nodesToAvoid.end()) {
                nodesToAvoid.insert(nextNode);
                nodes2process.push_back(nextNode);
            }
        }
    }
    return advanced;
}
