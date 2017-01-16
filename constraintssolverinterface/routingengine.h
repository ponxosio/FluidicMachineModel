#ifndef ROUTINGENGINE_H
#define ROUTINGENGINE_H

#include <unordered_map>
#include <stdexcept>

class RoutingEngine {
public:
    RoutingEngine(){}
    virtual ~RoutingEngine(){}

    virtual void setNumberContainer(int numberOfContainers) = 0;
    virtual std::unordered_map<std::string, long long> calculateNewRoute(const std::unordered_map<std::string, long long> & inputStates) throw(std::runtime_error) = 0;

};

#endif // ROUTINGENGINE_H
