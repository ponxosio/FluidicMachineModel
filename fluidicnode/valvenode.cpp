#include "valvenode.h"

ValveNode::ValveNode(const ValveNode & node) :
    FluidicMachineNode(node)
{
    this->actualPosition = node.actualPosition;
    copyTruthTable(node.truthTable);
}

ValveNode::ValveNode(int idNode, int numPins, const TruthTable & truthTable, std::shared_ptr<Function> valveRouteFunction) throw (std::invalid_argument) :
    FluidicMachineNode(idNode, numPins)
{
    if(valveRouteFunction->getAceptedOp() != route) {
        throw(std::invalid_argument("ValveNode's valveRouteFunction must be capable of route"));
    }
    this->actualPosition = 0;
    copyTruthTable(truthTable);

}

ValveNode::~ValveNode() {

}

void ValveNode::copyTruthTable(const TruthTable & table) {
    for (auto it = table.begin(); it != table.end(); ++it) {
        int position = it->first;
        std::vector<std::vector<int>> pins;

        for (auto itvalue = it->second.begin(); itvalue != it->second.end(); ++it) {
            std::vector<int> connectPinsVector(*itvalue);
            pins.push_back(connectPinsVector);
        }
        truthTable.insert(std::make_pair(position, pins));
    }
}

const std::vector<std::vector<int>> & ValveNode::getConnectedPins(int position) throw (std::invalid_argument) {
    auto it = truthTable.find(position);
    if (it != truthTable.end()) {
        return it->second;
    } else {
        throw (std::invalid_argument("unknow position " + std::to_string(position)));
    }
}
