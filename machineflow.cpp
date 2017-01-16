#include "machineflow.h"

MachineFlow::MachineFlow()
{

}

MachineFlow::~MachineFlow() {

}

void MachineFlow::addFlow(short int idSource, short int idTarget, float rate) {
    bool appended = false;
    for (auto it = actual.begin(); it != actual.end(); ++it) {
        appended = tryAppend(*it, idSource, idTarget, rate);
    }
    if (!appended) {
        std::deque<short int> tempPath;
        tempPath.push_back(idSource);
        tempPath.push_back(idTarget);
        actual.push_back(std::make_tuple(tempPath, rate));
    }
    mergeStacks();
}

void MachineFlow::removeFlow(short int idSource, short int idTarget) {
    addFlow(idSource, idTarget, 0.0);
}

const MachineFlow::FlowsVector & MachineFlow::updateFlows() {

}

bool MachineFlow::tryAppend(PathRateTuple & tuple, short int idSource, short int idTarget, float rate) {
    bool suceed = (rate == std::get<1>(tuple));
    if (suceed) {
        suceed = false;
        if (std::get<0>(tuple).back() == idSource) {
            suceed = true;
            std::get<0>(tuple).push_back(idTarget);
        } else if (std::get<0>(tuple).front() == idTarget) {
            suceed = true;
            std::get<0>(tuple).push_front(idSource);
        }
    }
    return succeed;
}

bool MachineFlow::areCompatible(const std::deque<short int> & queue1, const std::deque<short int> & queue2) {
    bool compatible = true;
    for (auto it1 = queue1.begin(); compatible && it1 != queue1.end(); ++it1) {
        bool finded = false;
        for (auto it2 = queue2.begin(); !finded && it2 != queue2.end(); ++it2) {
            finded = (*it1 == *it2);
        }
        compatible = finded;
    }
    return compatible;
}

void MachineFlow::removeZeroFlows(FlowsVector & flows) {
    for (auto it = flows.begin(); it != flows.end();) {
        if (std::get<1>(*it) == 0.0) {
            iter = flows.erase(it);
        } else {
            ++it;
        }
    }
}

void MachineFlow::mergeStacks() {
    FlowsVector temp;
    for(auto it_i = actual.begin(); it_i != actual.end(); ++ it_i) {
        for (auto it_j = actual.begin(); it_j != actual.end(); ++ it_j) {
            if (it_i != it_j &&
                std::get<1>(*it_i) == std::get<1>(*it_j) &&
                std::get<0>(*it_i).back() == std::get<0>(*it_j).front())
            {
                std::deque<short int> newQueue(std::get<0>(*it_i));
                newQueue.insert(newQueue.begin(), std::get<0>(*it_j).begin(), std::get<0>(*it_j).end());
                temp.push_back(std::make_tuple(newQueue, std::get<1>(*it_i)));
            } else {
                temp.push_back(*it_i);
                temp.push_back(*it_j);
            }
        }
    }
    actual = temp;
}
