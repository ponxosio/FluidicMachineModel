#include "pumppluginfunction.h"

PumpPluginFunction::PumpPluginFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration) :
    Function(factory), configurationObj(configuration)
{

}

PumpPluginFunction::~PumpPluginFunction() {

}

OperationType PumpPluginFunction::getAceptedOp() {
    return pump;
}

double PumpPluginFunction::doOperation(int nargs, va_list args) throw (std::invalid_argument) {
    if (!pluginPump) {
        pluginPump = factory->makePump(configurationObj);
    }

    if (nargs == 1) {
        //va_start(ap, args);
        int newState = va_arg(args, int);
        pluginPump->setPumpState(newState);
        //va_end(ap);
        return -1;
    } else {
        throw(std::invalid_argument(" doOperation() of PumpPluginFunction must receive 1 argument, received " + std::to_string(nargs)));
    }
}

double PumpPluginFunction::getMinVolume() {
    return 0.0;
}
