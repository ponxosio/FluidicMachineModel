#ifndef FLUIDICMACHINENODE_H
#define FLUIDICMACHINENODE_H

#include <stdexcept>
#include <cstdarg>
#include <unordered_map>

#include "graph/Node.h"
#include "fluidicedge/tubeedge.h"
#include "fluidicnode/functions/functionset.h"

class FluidicMachineNode : public Node
{
public:
    FluidicMachineNode();
    FluidicMachineNode (const FluidicMachineNode & node);
    FluidicMachineNode(int containerID, int numberOfPins);
    virtual ~FluidicMachineNode();

    void connectToPin(int pin, std::shared_ptr<TubeEdge> edge) throw (std::invalid_argument);

    bool canDoOperations(unsigned long mask);
    double doOperation(OperationType op, int nargs, va_list args) throw (std::invalid_argument);
    double getMinVolume(OperationType op) throw (std::invalid_argument);

    const FunctionSet getAvailableFunctions();
    void setFactory(std::shared_ptr<PluginAbstractFactory> factory);
    void setFactory(OperationType op, std::shared_ptr<PluginAbstractFactory> factory) throw(std::invalid_argument);

protected:
    int numberOfPins;
    std::unordered_map<int,std::shared_ptr<TubeEdge>> pinConnectionMap;
    FunctionSet availableFunctions;
};

#endif // FLUIDICMACHINENODE_H