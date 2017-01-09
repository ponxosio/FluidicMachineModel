#ifndef PLUGINABSTRACTFACTORY_H
#define PLUGINABSTRACTFACTORY_H

#include <memory>

#include "plugininterface/heaterpluginproduct.h"
#include "plugininterface/lightpluginproduct.h"
#include "plugininterface/odsensorpluginproduct.h"
#include "plugininterface/pumppluginproduct.h"
#include "plugininterface/stirerpluginproduct.h"
#include "plugininterface/valvepluginproduct.h"
#include "plugininterface/pluginconfiguration.h"

class PluginAbstractFactory {
public:
    PluginAbstractFactory(){}
    virtual ~PluginAbstractFactory(){}

    virtual std::shared_ptr<HeaterPluginProduct> makeHeater(const PluginConfiguration & configuration) = 0;
    virtual std::shared_ptr<LightPluginProduct> makeLight(const PluginConfiguration & configuration) = 0;
    virtual std::shared_ptr<OdSensorPluginProduct> makeOdSensor(const PluginConfiguration & configuration) = 0;
    virtual std::shared_ptr<PumpPluginProduct> makePump(const PluginConfiguration & configuration) = 0;
    virtual std::shared_ptr<StirerPluginProduct> makeStirer(const PluginConfiguration & configuration) = 0;
    virtual std::shared_ptr<ValvePluginProduct> makeValve(const PluginConfiguration & configuration) = 0;

};

#endif // PLUGINABSTRACTFACTORY_H
