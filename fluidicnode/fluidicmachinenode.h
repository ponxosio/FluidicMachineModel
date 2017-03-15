#ifndef FLUIDICMACHINENODE_H
#define FLUIDICMACHINENODE_H

#include <stdexcept>
#include <cstdarg>
#include <unordered_map>

#include <graph/Node.h>
#include <commonmodel/modelinterface/componentinterface.h>

#include "fluidicedge/tubeedge.h"

class FluidicMachineNode : public Node, public ComponentInterface
{
public:
    FluidicMachineNode();
    FluidicMachineNode (const FluidicMachineNode & node);
    FluidicMachineNode(int containerID, int numberOfPins);
    virtual ~FluidicMachineNode();

    void connectToPin(int pin, std::shared_ptr<TubeEdge> edge) throw (std::invalid_argument);
    std::tuple<int,int> getTubeIdConnectedToPin(int id) throw (std::invalid_argument);

    virtual bool canDoOperations(unsigned long mask);
    virtual MultiUnitsWrapper* doOperation(Function::OperationType op, int nargs, ...) throw (std::invalid_argument);
    virtual units::Volume getMinVolume(Function::OperationType op) throw (std::invalid_argument);

    void setFactory(std::shared_ptr<PluginAbstractFactory> factory);
    void setFactory(Function::OperationType op, std::shared_ptr<PluginAbstractFactory> factory) throw(std::invalid_argument);

    inline virtual Node* clone() = 0;

protected:
    int numberOfPins;
    std::unordered_map<int,std::tuple<int,int>> pinConnectionMap;
};

#endif // FLUIDICMACHINENODE_H
