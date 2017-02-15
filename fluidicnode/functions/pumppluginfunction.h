#ifndef PUMPPLUGINFUNCTION_H
#define PUMPPLUGINFUNCTION_H

#include <memory>
#include <cstdarg>

#include "fluidicnode/functions/function.h"
#include "plugininterface/pluginabstractfactory.h"
#include "plugininterface/pluginconfiguration.h"
#include "plugininterface/pumppluginproduct.h"

#include "fluidicmachinemodel_global.h"

class PUMPPLUGINFUNCTION_EXPORT PumpPluginFunction : public Function
{
public:
    PumpPluginFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration);
    virtual ~PumpPluginFunction();

    virtual OperationType getAceptedOp();
    virtual double doOperation(int nargs, va_list args) throw (std::invalid_argument);
    virtual double getMinVolume();

protected:
    PluginConfiguration configurationObj;
    std::shared_ptr<PumpPluginProduct> pluginPump;

};

#endif // PUMPPLUGINFUNCTION_H
