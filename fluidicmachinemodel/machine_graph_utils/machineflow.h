#ifndef MACHINEFLOW_H
#define MACHINEFLOW_H

#include <algorithm>
#include <deque>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <utils/memento.h>
#include <utils/units.h>
#include <utils/Utils.h>

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class MACHINE_FLOW_EXPORT MachineFlow
{
    friend class MachineFlowStringAdapter;

public:
    typedef std::tuple<std::deque<short int>,units::Volumetric_Flow> PathRateTuple;
    typedef std::vector<PathRateTuple> FlowsVector;

    MachineFlow();
    MachineFlow(const MachineFlow & mFlow);
    virtual ~MachineFlow();

    void addFlow(short int idSource, short int idTarget, units::Volumetric_Flow rate);
    void removeFlow(short int idSource, short int idTarget);
    const FlowsVector & updateFlows();

    std::string flowToStr();

    inline void clear() {
        previous.clear();
        actual.clear();
    }

    inline std::shared_ptr<Memento<MachineFlow>> createMemento() const {
        return std::make_shared<Memento<MachineFlow>>(*this);
    }

    inline void restoreMemento(const Memento<MachineFlow> & memento) {
        this->restoreState(memento.getState());
    }

protected:
    FlowsVector previous;
    FlowsVector actual;

    bool tryAppend(PathRateTuple & tuple, short int idSource, short int idTarget, units::Volumetric_Flow rate);
    bool areCompatible(const std::deque<short int> & queue1, const std::deque<short int> & queue2);
    void removeZeroFlows(FlowsVector & flows);
    void mergeStacks();

    void restoreState(const MachineFlow & state);
};

#endif // MACHINEFLOW_H
