#include "stirfunction.h"

StirFunction::StirFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration, double minVolume) :
    Function(factory), configurationObj(configuration)
{
    this->minVolume = minVolume;
}

StirFunction::~StirFunction() {

}

OperationType StirFunction::getAceptedOp() {
    return stir;
}

double StirFunction::doOperation(int nargs, va_list args) throw (std::invalid_argument) {
    if (!stirPlugin) {
        stirPlugin = factory->makeStirer(configurationObj);
    }

    if (nargs == 1) {
        //va_start(ap, args);
        double intensity = va_arg(args, double);
        stirPlugin->stir(intensity);
        //va_end(ap);
        return -1;
    } else {
        throw(std::invalid_argument(" doOperation() of StirFunction must receive 1 argument, received " + std::to_string(nargs)));
    }
}

double StirFunction::getMinVolume() {
    return minVolume;
}
