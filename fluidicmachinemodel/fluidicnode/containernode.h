#ifndef CONTAINERNODE_H
#define CONTAINERNODE_H

#include <utils/units.h>

#include "fluidicmachinemodel/fluidicnode/fluidicmachinenode.h"

class ContainerNode : public FluidicMachineNode
{
public:
    typedef enum ContainerType_ {
        open,
        close
    } ContainerType;

    ContainerNode();
    ContainerNode(const ContainerNode & node);
    ContainerNode(int idNode, int numPins, ContainerType containerType, units::Volume maxVolume);

    void addOperation(std::shared_ptr<Function> function);
    units::Volume getMaxVolume();
    units::Volume getActualVolume();
    void setActualVolume(units::Volume volume);
    ContainerType getContainerType();

    inline virtual Node* clone() {
        return new ContainerNode(*this);
    }
protected:
    units::Volume maxVolume;
    units::Volume actualVolume;
    ContainerType containerType;
};

#endif // CONTAINERNODE_H
