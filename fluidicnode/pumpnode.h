#ifndef PUMPNODE_H
#define PUMPNODE_H

#include <type_traits>

#include "fluidicnode/fluidicmachinenode.h"
#include "fluidicnode/functions/function.h"

typedef enum PumpType_ {
    unknow,
    unidirectional,
    bidirectional
} PumpType;

typedef enum FlowDirection_ {
    off,
    upstream,
    downstream
} FlowDirection;

class PumpNode : public FluidicMachineNode
{
public:
    PumpNode(const PumpNode & node);
    PumpNode(int idNode, PumpType type, std::shared_ptr<Function> pumpFunction);
    virtual ~PumpNode();

    //getters & setters
    PumpType getType();
    FlowDirection getFlowDirection();

protected:
    PumpType type;
    FlowDirection flowDirection;
};

#endif // PUMPNODE_H
