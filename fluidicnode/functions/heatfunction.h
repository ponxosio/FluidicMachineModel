#ifndef HEATFUNCTION_H
#define HEATFUNCTION_H

#include <memory>
#include <cstdarg>

#include "fluidicnode/functions/function.h"
#include "plugininterface/pluginabstractfactory.h"
#include "plugininterface/pluginconfiguration.h"
#include "plugininterface/heaterpluginproduct.h"

class HeatFunction : public Function
{
public:
    HeatFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration, double minVolume);
    virtual ~HeatFunction();

    virtual OperationType getAceptedOp();
    virtual double doOperation(int nargs, va_list args) throw (std::invalid_argument);
    virtual double getMinVolume();

protected:
    double minVolume;
    PluginConfiguration configurationObj;
    std::shared_ptr<HeaterPluginProduct> heaterPlugin;
};

#endif // HEATFUNCTION_H
