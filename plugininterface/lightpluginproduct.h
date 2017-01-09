#ifndef LIGHTPLUGINPRODUCT_H
#define LIGHTPLUGINPRODUCT_H


class LightPluginProduct
{
public:
    LightPluginProduct(){}
    virtual ~LightPluginProduct(){}

    virtual void applyLight(double intensity, double wavelenght) = 0;
};

#endif // LIGHTPLUGINPRODUCT_H
