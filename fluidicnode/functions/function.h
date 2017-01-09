#ifndef FUNCTION_H
#define FUNCTION_H

#include <cstdarg>
#include <stdexcept>

#include "plugininterface/pluginabstractfactory.h"

typedef enum _OperationType {
    route,
    pump,
    heat,
    apply_light,
    measure_od,
    stir
} OperationType;

class Function
{
public:
    Function(std::shared_ptr<PluginAbstractFactory> factory) {
        this->factory = factory;
    }
    virtual ~Function(){}

    virtual OperationType getAceptedOp() = 0;
    virtual double doOperation(int nargs, va_list args) throw(std::invalid_argument) = 0;
    virtual double getMinVolume() = 0;

    inline void setFactory(std::shared_ptr<PluginAbstractFactory> factory) {
        this->factory = factory;
    }

protected:
    std::shared_ptr<PluginAbstractFactory> factory;
};

#endif // FUNCTION_H
