#include "pumpnode.h"

PumpNode::PumpNode(const PumpNode & node) :
    FluidicMachineNode(node)
{
    this->type = node.type;
    this->flowDirection = node.flowDirection;
}

PumpNode::PumpNode(int idNode, PumpType type, std::shared_ptr<Function> pumpFunction) throw (std::invalid_argument) :
    FluidicMachineNode(idNode, 2)
{
   if (pumpFunction->getAceptedOp() != pump) {
        throw (std::invalid_argument("PumpNode's pumpFunction must be capable of pump"));
   }

    this->type = type;
    this->flowDirection = off;
    this->availableFunctions.addOperation(pumpFunction);
}

PumpNode::~PumpNode() {

}

PumpType PumpNode::getType() {
    return type;
}

FlowDirection PumpNode::getFlowDirection() {
    return flowDirection;
}
