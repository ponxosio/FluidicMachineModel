#ifndef MACHINEGRAPHITERATOR_H
#define MACHINEGRAPHITERATOR_H

#include <unordered_set>
#include <vector>

#include "fluidicmachinemodel/machinegraph.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class MACHINEGRAPHITERATOR_EXPORT MachineGraphIterator
{
public:
    MachineGraphIterator(int startNode, std::shared_ptr<MachineGraph> graph);
    MachineGraphIterator(int startNode, std::shared_ptr<MachineGraph> graph, const std::unordered_set<int> & nodesToAvoid);
    virtual ~MachineGraphIterator();

    virtual bool advance();

    inline int getActualNode() {
        return actualNode;
    }

protected:
    int actualNode;
    std::shared_ptr<MachineGraph> graphPtr;
    std::unordered_set<int> nodesToAvoid;
    std::vector<int> nodes2process;

};

#endif // MACHINEGRAPHITERATOR_H
