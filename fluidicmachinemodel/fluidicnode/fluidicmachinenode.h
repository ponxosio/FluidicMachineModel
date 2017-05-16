#ifndef FLUIDICMACHINENODE_H
#define FLUIDICMACHINENODE_H

#include <stdexcept>
#include <cstdarg>
#include <unordered_map>

#include <graph/Node.h>
#include <commonmodel/modelinterface/componentinterface.h>

#include "fluidicmachinemodel/fluidicedge/tubeedge.h"

class FluidicMachineNode : public Node, public ComponentInterface
{
public:
    FluidicMachineNode();
    FluidicMachineNode (const FluidicMachineNode & node);
    FluidicMachineNode(int containerID, int numberOfPins);
    virtual ~FluidicMachineNode();

    void connectToPin(int pin, std::shared_ptr<TubeEdge> edge) throw (std::invalid_argument);
    std::tuple<int,int> getTubeIdConnectedToPin(int id) throw (std::invalid_argument);

    virtual bool canDoOperations(unsigned long mask) const;
    virtual std::shared_ptr<MultiUnitsWrapper> doOperation(Function::OperationType op, int nargs, ...) throw (std::invalid_argument);

    virtual bool inWorkingRange(Function::OperationType op, int nargs, ...) const throw (std::invalid_argument);
    virtual const std::shared_ptr<const ComparableRangeInterface> getComparableWorkingRange(Function::OperationType op) const;

    virtual units::Volume getMinVolume(Function::OperationType op) const throw (std::invalid_argument);

    void setFactory(std::shared_ptr<PluginAbstractFactory> factory);
    void setFactory(Function::OperationType op, std::shared_ptr<PluginAbstractFactory> factory) throw(std::invalid_argument);

    inline virtual Node* clone() const = 0;

protected:
    int numberOfPins;
    std::unordered_map<int,std::tuple<int,int>> pinConnectionMap;
};

#endif // FLUIDICMACHINENODE_H
