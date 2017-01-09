#include "lightfunction.h"

LightFunction::LightFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration, double minVolume) :
    Function(factory) , configurationObj(configuration)
{
    this->minVolume = minVolume;
}

LightFunction::~LightFunction() {

}

OperationType LightFunction::getAceptedOp() {
    return apply_light;
}

double LightFunction::doOperation(int nargs, va_list args) throw (std::invalid_argument) {
    if (!lightPlugin) {
        lightPlugin = factory->makeLight(configurationObj);
    }

    if (nargs == 2) {
        //va_start(ap, args);
        double intensity = va_arg(args, double);
        double wavelength = va_arg(args, double);
        lightPlugin->applyLight(intensity, wavelength);
        //va_end(ap);
        return -1;
    } else {
        throw(std::invalid_argument(" doOperation() of LightFunction must receive 2 argument, received " + std::to_string(nargs)));
    }
}

double LightFunction::getMinVolume() {
    return minVolume;
}
