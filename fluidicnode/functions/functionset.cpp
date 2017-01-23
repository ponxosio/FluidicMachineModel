#include "functionset.h"

FunctionSet::FunctionSet()
{

}

FunctionSet::FunctionSet(const FunctionSet & set) :
    aceptedFunctions(set.aceptedFunctions)
{
    for (auto it: set.functionsMap) {
        functionsMap.insert(std::make_pair(it.first, it.second));
    }
}

FunctionSet::~FunctionSet() {

}

bool FunctionSet::canDoOperations(unsigned long mask) {
    unsigned long posibleops = aceptedFunctions.to_ulong();
    return ((mask & posibleops) == mask);
}

double FunctionSet::doOperation(OperationType op, int nargs, va_list args) throw (std::invalid_argument) {
    int castop = (int) op;

    auto it = functionsMap.find(castop);
    if (it != functionsMap.end()) {
        std::shared_ptr<Function> function = it->second;
        return function->doOperation(nargs, args);
    } else {
        throw(std::invalid_argument("funtion " + std::to_string((int) op) + " not present"));
    }
}

double FunctionSet::getMinVolume(OperationType op) throw (std::invalid_argument) {
    int castop = (int) op;

    auto it = functionsMap.find(castop);
    if (it != functionsMap.end()) {
        std::shared_ptr<Function> function = it->second;
        return function->getMinVolume();
    } else {
        throw(std::invalid_argument("funtion " + std::to_string((int) op) + " not present"));
    }
}

void FunctionSet::addOperation(std::shared_ptr<Function> function) {
    int castop = (int) function->getAceptedOp();

    auto it = functionsMap.find(castop);
    if (it != functionsMap.end()) {
        it->second = function;
    } else {
        functionsMap.insert(std::make_pair(castop, function));
    }
    aceptedFunctions[castop] = 1;
}

void FunctionSet::setFactory(std::shared_ptr<PluginAbstractFactory> factory) {
    for (auto it : functionsMap) {
        it.second->setFactory(factory);
    }
}

void FunctionSet::setFactory(OperationType op, std::shared_ptr<PluginAbstractFactory> factory) throw(std::invalid_argument) {
    int castop = (int) op;

    auto it = functionsMap.find(castop);
    if (it != functionsMap.end()) {
        std::shared_ptr<Function> function = it->second;
        function->setFactory(factory);
    } else {
        throw(std::invalid_argument("funtion " + std::to_string((int) op) + " not present"));
    }
}
