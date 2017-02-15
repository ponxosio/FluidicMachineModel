#include "pumpnode.h"

PumpNode::PumpNode(const PumpNode & node) :
    FluidicMachineNode(node)
{
    this->type = node.type;
}

PumpNode::PumpNode(int idNode, int numPins, PumpType type, std::shared_ptr<Function> pumpFunction) throw (std::invalid_argument) :
    FluidicMachineNode(idNode, numPins)
{
   if (pumpFunction->getAceptedOp() != pump) {
        throw (std::invalid_argument("PumpNode's pumpFunction must be capable of pump"));
   }

    this->type = type;
    this->availableFunctions.addOperation(pumpFunction);
}

PumpNode::~PumpNode() {

}

PumpNode::PumpType PumpNode::getType() {
    return type;
}

