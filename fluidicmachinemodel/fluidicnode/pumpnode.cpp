#include "pumpnode.h"

PumpNode::PumpNode(const PumpNode & node) :
    FluidicMachineNode(node)
{
    this->type = node.type;
}

PumpNode::PumpNode(int idNode, int numPins, PumpType type, std::shared_ptr<Function> pumpFunction) throw (std::invalid_argument) :
    FluidicMachineNode(idNode, numPins)
{
   if (pumpFunction->getAceptedOp() != Function::pump) {
        throw (std::invalid_argument("PumpNode's pumpFunction must be capable of pump"));
   }

    this->type = type;
    ComponentInterface::availableFunctions.addOperation(pumpFunction);
}

PumpNode::~PumpNode() {

}

PumpNode::PumpType PumpNode::getType() {
    return type;
}

std::string PumpNode::toText() {
    std::string name = (type == bidirectional ? "_BIDIRECTIONAL_PUMP" : "_UNIDIRECTIONAL_PUMP");
    std::string vuelta = std::to_string(containerID) + "[label=\"" + std::to_string(containerID) + name + "\"];";
    return vuelta;
}

