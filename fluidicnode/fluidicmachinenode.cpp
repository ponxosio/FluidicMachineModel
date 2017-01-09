#include "fluidicmachinenode.h"

FluidicMachineNode::FluidicMachineNode() :
    Node()
{
    this->numberOfPins = 0;
}

FluidicMachineNode::FluidicMachineNode(const FluidicMachineNode & node) :
    Node(node), availableFunctions(node.availableFunctions)
{
    this->numberOfPins = node.numberOfPins;

    for (auto it : node.pinConnectionMap) {
        this->pinConnectionMap.insert(std::make_pair(it.first, it.second));
    }
}

FluidicMachineNode::FluidicMachineNode(int containerID, int numberOfPins) :
    Node(containerID)
{
    this->numberOfPins = numberOfPins;
}

FluidicMachineNode::~FluidicMachineNode() {

}

void FluidicMachineNode::connectToPin(int pin, std::shared_ptr<TubeEdge> edge) throw (std::invalid_argument)
{
    if (pin < numberOfPins) {
        auto it = pinConnectionMap.find(pin);
        if (it != pinConnectionMap.end()) {
            it->second = edge;
        } else {
            pinConnectionMap.insert(std::make_pair(pin, edge));
        }
    } else {
        throw(std::invalid_argument("the node does not has pin number " + std::to_string(pin)));
    }
}

const FunctionSet FluidicMachineNode::getAvailableFunctions() {
    return availableFunctions;
}

bool FluidicMachineNode::canDoOperations(unsigned long mask) {
    return availableFunctions.canDoOperations(mask);
}

double FluidicMachineNode::doOperation(OperationType op, int nargs, va_list args) throw (std::invalid_argument) {
    try {
        return availableFunctions.doOperation(op, nargs, args);
    } catch (std::invalid_argument & e) {
        throw (std::invalid_argument(" container " + std::to_string(containerID) + " fail to doOperation, " + std::string(e.what())));
    }
}

double FluidicMachineNode::getMinVolume(OperationType op) throw (std::invalid_argument) {
    try {
        return availableFunctions.getMinVolume(op);
    } catch (std::invalid_argument & e) {
        throw (std::invalid_argument(" container " + std::to_string(containerID) + " fail to getMinVolume, " + std::string(e.what())));
    }
}

void FluidicMachineNode::setFactory(std::shared_ptr<PluginAbstractFactory> factory) {
    availableFunctions.setFactory(factory);
}

void FluidicMachineNode::setFactory(OperationType op, std::shared_ptr<PluginAbstractFactory> factory) throw(std::invalid_argument) {
    try {
        availableFunctions.setFactory(op, factory);
    } catch (std::invalid_argument & e) {
        throw (std::invalid_argument(" container " + std::to_string(containerID) + " fail to setFactory, " + std::string(e.what())));
    }
}
