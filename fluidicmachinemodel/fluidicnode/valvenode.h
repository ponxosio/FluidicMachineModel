#ifndef VALVENODE_H
#define VALVENODE_H

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <type_traits>

#include "fluidicmachinemodel/fluidicnode/fluidicmachinenode.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class VALVENODE_EXPORT ValveNode : public FluidicMachineNode
{
public:
    typedef std::unordered_map<int, std::vector<std::unordered_set<int>>> TruthTable;

    ValveNode(const ValveNode & node);
    ValveNode(int idNode, int numPins, const TruthTable & truthTable, std::shared_ptr<Function> valveRouteFunction) throw (std::invalid_argument);
    virtual ~ValveNode();

    const std::vector<std::unordered_set<int>> & getConnectedPins(int position) throw (std::invalid_argument);
    std::set<int> getValvePossibleValues();

    virtual std::string toText();

    inline int getNumOfPositions() {
        return truthTable.size();
    }
    inline const TruthTable & getTruthTable() {
        return truthTable;
    }
    inline virtual Node* clone() const {
        return new ValveNode(*this);
    }

    inline int getActualPosition() {
        return actualPosition;
    }
    inline void setActualPosition(int actualPosition) {
        this->actualPosition = actualPosition;
    }

    inline virtual units::Volume getActualVolume() const {
        return 0 * units::ml;
    }

protected:
    int actualPosition;
    TruthTable truthTable;

    void copyTruthTable(const TruthTable & table);
    void checkTruthTableFormat(const TruthTable & table) throw(std::invalid_argument);
};

#endif // VALVENODE_H
