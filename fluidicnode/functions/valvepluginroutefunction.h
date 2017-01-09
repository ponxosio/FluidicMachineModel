#ifndef VALVEPLUGINROUTEFUNCTION_H
#define VALVEPLUGINROUTEFUNCTION_H

#include <memory>

#include "fluidicnode/functions/function.h"
#include "plugininterface/pluginabstractfactory.h"
#include "plugininterface/pluginconfiguration.h"
#include "plugininterface/valvepluginproduct.h"

class ValvePluginRouteFunction : public Function
{
public:
    ValvePluginRouteFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration);
    virtual ~ValvePluginRouteFunction();

    virtual OperationType getAceptedOp();
    virtual double doOperation(int nargs, va_list args) throw(std::invalid_argument);
    virtual double getMinVolume();

protected:
    PluginConfiguration configurationObj;
    std::shared_ptr<ValvePluginProduct> valvePlugin;
};

#endif // VALVEPLUGINROUTEFUNCTION_H
