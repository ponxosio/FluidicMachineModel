#ifndef MACHINEGRAPHPRESSUREITERATOR_H
#define MACHINEGRAPHPRESSUREITERATOR_H

#include "machine_graph_utils/machinegraphiterator.h"

class MachineGraphPressureIterator : public MachineGraphIterator
{
public:
    MachineGraphPressureIterator(int startNode, std::shared_ptr<MachineGraph> graph);
    MachineGraphPressureIterator(int startNode, std::shared_ptr<MachineGraph> graph, const std::unordered_set<int> & nodesToAvoid);
    virtual ~MachineGraphPressureIterator();

    virtual bool advance();
};

#endif // MACHINEGRAPHPRESSUREITERATOR_H
