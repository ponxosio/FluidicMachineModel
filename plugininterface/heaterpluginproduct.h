#ifndef HEATERPLUGINPRODUCT_H
#define HEATERPLUGINPRODUCT_H


class HeaterPluginProduct
{
public:
    HeaterPluginProduct(){}
    virtual ~HeaterPluginProduct(){}

    virtual void changeTemperature(double value) = 0;
};

#endif // HEATERPLUGINPRODUCT_H
