#include "heatfunction.h"

HeatFunction::HeatFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration, double minVolume) :
    Function(factory), configurationObj(configuration)
{
    this->minVolume = minVolume;
}

HeatFunction::~HeatFunction() {

}

OperationType HeatFunction::getAceptedOp() {
    return heat;
}

double HeatFunction::doOperation(int nargs, va_list args) throw (std::invalid_argument) {
    if (!heaterPlugin) {
        heaterPlugin = factory->makeHeater(configurationObj);
    }

    if (nargs == 1) {
        //va_start(ap, args);
        double temperature = va_arg(args, double);
        heaterPlugin->changeTemperature(temperature);
        //va_end(ap);
        return -1;
    } else {
        throw(std::invalid_argument(" doOperation() of HeatFunction must receive 1 argument, received " + std::to_string(nargs)));
    }
}

double HeatFunction::getMinVolume() {
    return minVolume;
}
