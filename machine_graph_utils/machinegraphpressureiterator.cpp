#include "machinegraphpressureiterator.h"

MachineGraphPressureIterator::MachineGraphPressureIterator(int startNode, std::shared_ptr<MachineGraph> graph) :
    MachineGraphIterator(startNode, graph)
{

}

MachineGraphPressureIterator::MachineGraphPressureIterator(int startNode, std::shared_ptr<MachineGraph> graph, const std::unordered_set<int> & nodesToAvoid) :
    MachineGraphIterator(startNode, graph, nodesToAvoid)
{

}

MachineGraphPressureIterator::~MachineGraphPressureIterator() {

}

bool MachineGraphPressureIterator::advance() {
    bool advanced = false;
    if (!nodes2process.empty()) {
        advanced = true;

        actualNode = nodes2process.back();
        nodes2process.pop_back();

        if (!graphPtr->isOpenContainer(actualNode)) {
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
    }
    return advanced;
}
