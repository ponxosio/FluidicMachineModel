#include "containernode.h"

ContainerNode::ContainerNode() :
    FluidicMachineNode()
{
    maxVolume = 0.0;
    actualVolume = 0.0;
    containerType = open;
}

ContainerNode::ContainerNode(const ContainerNode & node) :
    FluidicMachineNode(node)
{
    this->maxVolume = node.maxVolume;
    this->actualVolume = node.actualVolume;
    this->containerType = node.containerType;
}

ContainerNode::ContainerNode(int idNode, int numPins, ContainerType containerType, units::Volume maxVolume) :
    FluidicMachineNode(idNode, numPins)
{
    this->containerType = containerType;
    this->maxVolume = maxVolume;
    this->actualVolume = 0.0;
}

void ContainerNode::addOperation(std::shared_ptr<Function> function) {
    ComponentInterface::availableFunctions.addOperation(function);
}

const units::Volume & ContainerNode::getMaxVolume() const {
    return maxVolume;
}

const ContainerNode::ContainerType & ContainerNode::getContainerType() const {
    return containerType;
}

void ContainerNode::setActualVolume(units::Volume volume) {
    actualVolume = volume;
}

std::string ContainerNode::toText() {
    std::string name = (containerType == open ? "_OPEN_CONTAINER" : "_CLOSE_CONTAINER");
    std::string vuelta = std::to_string(containerID) + "[label=\"" + std::to_string(containerID) + name + "\"];";
    return vuelta;
}
