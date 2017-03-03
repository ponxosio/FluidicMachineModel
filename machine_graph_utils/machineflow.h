#ifndef MACHINEFLOW_H
#define MACHINEFLOW_H

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <deque>
#include <algorithm>

#include <utils/Utils.h>

#include "fluidicmachinemodel_global.h"

class MACHINE_FLOW_EXPORT MachineFlow
{
public:
    typedef std::tuple<std::deque<short int>,float> PathRateTuple;
    typedef std::vector<PathRateTuple> FlowsVector;

    MachineFlow();
    virtual ~MachineFlow();

    void addFlow(short int idSource, short int idTarget, float rate);
    void removeFlow(short int idSource, short int idTarget);
    const FlowsVector & updateFlows();

    std::string flowToStr();

    inline void clear() {
        previous.clear();
        actual.clear();
    }

protected:
    FlowsVector previous;
    FlowsVector actual;

    bool tryAppend(PathRateTuple & tuple, short int idSource, short int idTarget, float rate);
    bool areCompatible(const std::deque<short int> & queue1, const std::deque<short int> & queue2);
    void removeZeroFlows(FlowsVector & flows);
    void mergeStacks();
};

#endif // MACHINEFLOW_H
