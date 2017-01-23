#ifndef MACHINEGRAPHGENERATOR_H
#define MACHINEGRAPHGENERATOR_H

#include <vector>
#include <memory>

#include "fluidicedge/tubeedge.h"
#include "fluidicnode/fluidicmachinenode.h"
#include "fluidicnode/valvenode.h"
#include "graph/Graph.h"
#include "util/sequence.h"


class MachineGraphGenerator
{
public:
    typedef Graph<FluidicMachineNode,TubeEdge> MachineGraph;
    typedef std::shared_ptr<MachineGraph> GraphPtr;

    MachineGraphGenerator(GraphPtr graph,
                          const std::vector<int> * pumps,
                          const std::vector<int> * valves);
    virtual ~MachineGraphGenerator();

    bool hasNext();
    std::vector<GraphPtr> next() throw(std::invalid_argument);

protected:
    GraphPtr graph;
    int maxSequenceIndex;
    const std::vector<int> * pumps;
    const std::vector<int> * valves;
    std::vector<Sequence*> statesSequenceVector;

    void advanceSequence();
    bool isValidGraph(GraphPtr graph);
    std::vector<short int> getNextStateFromSequence();
    GraphPtr createGraphForState(const std::vector<short int> & estates);
    std::vector<GraphPtr> calculateSubgraphs(GraphPtr graph);
};

#endif // MACHINEGRAPHGENERATOR_H
