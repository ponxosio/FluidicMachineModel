#ifndef LIGHTFUNCTION_H
#define LIGHTFUNCTION_H

#include <memory>
#include <cstdarg>

#include "fluidicnode/functions/function.h"
#include "plugininterface/pluginabstractfactory.h"
#include "plugininterface/pluginconfiguration.h"
#include "plugininterface/lightpluginproduct.h"

class LightFunction : public Function
{
public:
    LightFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration, double minVolume);
    virtual ~LightFunction();

    virtual OperationType getAceptedOp();
    virtual double doOperation(int nargs, va_list args) throw (std::invalid_argument);
    virtual double getMinVolume();

protected:
    double minVolume;
    PluginConfiguration configurationObj;
    std::shared_ptr<LightPluginProduct> lightPlugin;
};

#endif // LIGHTFUNCTION_H
