#ifndef ROUTINGENGINE_H
#define ROUTINGENGINE_H

#include <unordered_map>
#include <stdexcept>

class RoutingEngine {
public:
    RoutingEngine(){}
    virtual ~RoutingEngine(){}

    virtual bool calculateNewRoute(const std::unordered_map<std::string, long long> & inputStates,
                                   std::unordered_map<std::string, long long> & outStates) throw(std::runtime_error) = 0;

};

#endif // ROUTINGENGINE_H
