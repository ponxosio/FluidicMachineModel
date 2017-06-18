#ifndef CONTAINERNODE_H
#define CONTAINERNODE_H

#include <utils/units.h>

#include "fluidicmachinemodel/fluidicnode/fluidicmachinenode.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class CONTAINERNODE_EXPORT ContainerNode : public FluidicMachineNode
{
public:
    typedef enum {
        open,
        close,
        unknow
    } ContainerType;

    ContainerNode();
    ContainerNode(const ContainerNode & node);
    ContainerNode(int idNode, int numPins, ContainerType containerType, units::Volume maxVolume);

    void addOperation(std::shared_ptr<Function> function);

    const units::Volume & getMaxVolume() const;

    void setActualVolume(units::Volume volume);

    const ContainerType & getContainerType() const;

    virtual std::string toText();

    inline virtual units::Volume getActualVolume() const {
        return actualVolume;
    }

    inline virtual Node* clone() const {
        return new ContainerNode(*this);
    }
protected:
    units::Volume maxVolume;
    units::Volume actualVolume;
    ContainerType containerType;
};

#endif // CONTAINERNODE_H
