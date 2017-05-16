#ifndef PUMPNODE_H
#define PUMPNODE_H

#include <type_traits>

#include <commonmodel/functions/function.h>

#include "fluidicmachinemodel/fluidicnode/fluidicmachinenode.h"

class PumpNode : public FluidicMachineNode
{
public:
    typedef enum PumpType_ {
        unknow,
        unidirectional,
        bidirectional
    } PumpType;

    PumpNode(const PumpNode & node);
    PumpNode(int idNode, int numPins, PumpType type, std::shared_ptr<Function> pumpFunction) throw (std::invalid_argument);
    virtual ~PumpNode();

    //getters & setters
    PumpType getType();

    inline virtual Node* clone() const {
        return new PumpNode(*this);
    }

protected:
    PumpType type;
};

#endif // PUMPNODE_H
