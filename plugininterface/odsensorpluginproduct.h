#ifndef ODSENSORPLUGINPRODUCT_H
#define ODSENSORPLUGINPRODUCT_H


class OdSensorPluginProduct
{
public:
    OdSensorPluginProduct(){}
    virtual ~OdSensorPluginProduct(){}

    virtual double measureOd() = 0;
};

#endif // ODSENSORPLUGINPRODUCT_H
