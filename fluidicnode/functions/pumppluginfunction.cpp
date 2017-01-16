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

    if (nargs == 2) {
        //va_start(ap, args);
        int dir = va_arg(args, int);
        float rate = va_arg(args, float);
        pluginPump->setPumpState(dir, rate);
        //va_end(ap);
        return -1;
    } else {
        throw(std::invalid_argument(" doOperation() of PumpPluginFunction must receive 2 arguments, received " + std::to_string(nargs)));
    }
}

double PumpPluginFunction::getMinVolume() {
    return 0.0;
}
