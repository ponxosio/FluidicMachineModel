#ifndef VALVENODE_H
#define VALVENODE_H

#include <unordered_map>
#include <vector>
#include <type_traits>

#include "fluidicnode/fluidicmachinenode.h"

class ValveNode : public FluidicMachineNode
{
public:
    typedef std::unordered_map<int, std::vector<std::vector<int>>> TruthTable;

    ValveNode(const ValveNode & node);
    ValveNode(int idNode, int numPins, const TruthTable & truthTable, std::shared_ptr<Function> valveRouteFunction) throw (std::invalid_argument);
    virtual ~ValveNode();


    const std::vector<std::vector<int>> & getConnectedPins(int position) throw (std::invalid_argument);
    std::vector<int> getValvePossibleValues();

    inline int getNumOfPositions() {
        return truthTable.size();
    }

    inline const TruthTable & getTruthTable() {
        return truthTable;
    }

protected:
    int actualPosition;
    TruthTable truthTable;

    void copyTruthTable(const TruthTable & table);
};

#endif // VALVENODE_H
