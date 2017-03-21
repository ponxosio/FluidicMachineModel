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
    if(valveRouteFunction->getAceptedOp() != Function::route) {
        throw(std::invalid_argument("ValveNode's valveRouteFunction must be capable of route"));
    }
    ComponentInterface::availableFunctions.addOperation(valveRouteFunction);

    try {
        checkTruthTableFormat(truthTable);
        this->actualPosition = truthTable.begin()->first;
        copyTruthTable(truthTable);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument(std::string(e.what())));
    }
}

ValveNode::~ValveNode() {

}

const std::vector<std::unordered_set<int>> & ValveNode::getConnectedPins(int position) throw (std::invalid_argument) {
    auto it = truthTable.find(position);
    if (it != truthTable.end()) {
        return it->second;
    } else {
        throw (std::invalid_argument("unknow position " + std::to_string(position)));
    }
}

std::set<int> ValveNode::getValvePossibleValues() {
    std::set<int> possibleValues;
    for(const auto it: truthTable) {
        possibleValues.insert(it.first);
    }
    return possibleValues;
}

void ValveNode::copyTruthTable(const TruthTable & table) {
    for (auto it = table.begin(); it != table.end(); ++it) {
        int position = it->first;
        const std::vector<std::unordered_set<int>> & connectedPins = it->second;

        std::vector<std::unordered_set<int>> pins;
        for (auto itvalue = connectedPins.begin(); itvalue != connectedPins.end(); ++itvalue) {
            std::unordered_set<int> connectPinsVector(*itvalue);
            pins.push_back(connectPinsVector);
        }
        truthTable.insert(std::make_pair(position, pins));
    }
}

void ValveNode::checkTruthTableFormat(const TruthTable & table) throw(std::invalid_argument) {
    if (table.empty()) {
        throw(std::invalid_argument("Empty Truth Table"));
    }

    for (const auto & pair: table) {
        int positionNum = pair.first;
        const std::vector<std::unordered_set<int>> & connectedPinsVector = pair.second;

        for (int i = 0; i < connectedPinsVector.size(); i++) {
            const std::unordered_set<int> & connectedPins = connectedPinsVector[i];
            if (connectedPins.size() < 2) {
                throw(std::invalid_argument("Position: " + std::to_string(positionNum) +
                                            ", connectedPins combination:" + std::to_string(i) +
                                            " must has at least 2 pins to connect and has " + std::to_string(connectedPins.size())));
            }
        }
    }
}
