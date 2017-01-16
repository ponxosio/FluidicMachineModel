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
}

void MachineFlow::removeFlow(short int idSource, short int idTarget) {
    addFlow(idSource, idTarget, 0.0);
}

const MachineFlow::FlowsVector & MachineFlow::updateFlows() {
    mergeStacks();
    for(auto it_actual = actual.begin(); it_actual != actual.end(); ++it_actual) {
        bool compatible = false;
        for (auto it_previous = previous.begin(); it_previous != previous.end(); ++it_previous) {
            if (areCompatible(std::get<0>(*it_actual), std::get<0>(*it_previous))) {
                compatible = true;
                std::get<1>(*it_previous) = std::get<1>(*it_asctual);
            }
        }
        if (!compatible) {
            previous.push_back(std::make_tuple(std::get<0>(*it_actual), std::get<1>(*it_actual)));
        }
    }
    actual.clear();
    removeZeroFlows(previous);
    return previous;
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
    bool changes = false;
    do {
        for(auto it_i = actual.begin(); it_i != actual.end(); ) {
            for (auto it_j = it_i + 1; it_j != actual.end(); ++ it_j) {
                double rate_i = std::get<1>(*it_i);
                double rate_j = std::get<1>(*it_j);
                if (rate_i == rate_j) {
                    std::deque<short int> stack_i = std::get<0>(*it_i);
                    std::deque<short int> stack_j = std::get<0>(*it_j);
                    if (stack_i.front() == stack_j.back()) {
                        changes = true;
                        stack_j.insert(stack_j.back(),stack_i.begin() + 1, stack_i.end());
                    } else if (stack_i.back() == stack_j.front()) {
                        changes = true;
                        stack_j.insert(stack_j.begin(),stack_i.begin(), stack_i.end() - 1);
                    }
                }
            }

            if (changes) {
                it_i = actual.erase(it_i);
            } else {
                ++it_i;
            }
        }
    } while(changes);
}
