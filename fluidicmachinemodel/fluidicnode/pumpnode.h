#ifndef PUMPNODE_H
#define PUMPNODE_H

#include <type_traits>

#include <commonmodel/functions/function.h>

#include "fluidicmachinemodel/fluidicnode/fluidicmachinenode.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class PUMPNODE_EXPORT PumpNode : public FluidicMachineNode
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

    virtual std::string toText();

    inline virtual Node* clone() const {
        return new PumpNode(*this);
    }

    inline virtual units::Volume getActualVolume() const {
        return 0 * units::ml;
    }

protected:
    PumpType type;
};

#endif // PUMPNODE_H
