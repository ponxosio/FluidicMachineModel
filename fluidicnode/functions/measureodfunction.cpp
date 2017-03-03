#include "measureodfunction.h"

MeasureOdFunction::MeasureOdFunction(std::shared_ptr<PluginAbstractFactory> factory, const PluginConfiguration & configuration, double minVolume) :
    Function(factory), configurationObj(configuration)
{
    this->minVolume = minVolume;
}

MeasureOdFunction::~MeasureOdFunction() {

}

OperationType MeasureOdFunction::getAceptedOp() {
    return measure_od;
}

double MeasureOdFunction::doOperation(int nargs, va_list args) throw (std::invalid_argument) {
    if (!odSensoPlugin) {
        odSensoPlugin = factory->makeOdSensor(configurationObj);
    }

    if (nargs == 0) {
        //va_start(args, nargs);
        //va_end(args);
        return odSensoPlugin->measureOd();
    } else {
        throw(std::invalid_argument(" doOperation() of MeasureOdFunction must receive 0 argument, received " + std::to_string(nargs)));
    }
}

double MeasureOdFunction::getMinVolume() {
    return minVolume;
}
