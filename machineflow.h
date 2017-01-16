#ifndef MACHINEFLOW_H
#define MACHINEFLOW_H

#include <unordered_map>
#include <vector>
#include <deque>
#include <algorithm>

#include "util/Utils.h"

class MachineFlow
{
public:
    typedef std::vector<PathRateTuple> FlowsVector;
    typedef std::tuple<std::deque<short int>,float> PathRateTuple;

    MachineFlow();
    virtual ~MachineFlow();

    void addFlow(short int idSource, short int idTarget, float rate);
    void removeFlow(short int idSource, short int idTarget);
    const FlowsVector & updateFlows();

protected:
    FlowsVector previous;
    FlowsVector actual;

    bool tryAppend(PathRateTuple & tuple, short int idSource, short int idTarget, float rate);
    bool areCompatible(const std::deque<short int> & queue1, const std::deque<short int> & queue2);
    void removeZeroFlows(FlowsVector & flows);
    void mergeStacks();
};

#endif // MACHINEFLOW_H
