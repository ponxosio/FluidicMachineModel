#ifndef CONTAINERNODE_H
#define CONTAINERNODE_H

#include "fluidicnode/fluidicmachinenode.h"

typedef enum ContainerType_ {
    open,
    close
} ContainerType;

class ContainerNode : public FluidicMachineNode
{
public:
    ContainerNode();
    ContainerNode(const ContainerNode & node);
    ContainerNode(int idNode, ContainerType containerType, double maxVolume);

    void addOperation(std::shared_ptr<Function> function);
    double getMaxVolume();
    double getActualVolume();
    void setActualVolume(double volume);
    ContainerType getContainerType();

protected:
    double maxVolume;
    double actualVolume;
    ContainerType containerType;
};

#endif // CONTAINERNODE_H
