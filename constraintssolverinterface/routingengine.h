#ifndef ROUTINGENGINE_H
#define ROUTINGENGINE_H

#include <vector>

class RoutingEngine {
public:
    RoutingEngine(){}
    virtual ~RoutingEngine(){}

    virtual void setNumberContainer(int numberOfContainers) = 0;
    virtual bool calculateNewRoute(std::vector<int> & containers, std::vector<int> & tubes, std::vector<int> & pumps, std::vector<int> & valves) = 0;

};

#endif // ROUTINGENGINE_H
