#ifndef STIRERPLUGINPRODUCT_H
#define STIRERPLUGINPRODUCT_H


class StirerPluginProduct
{
public:
    StirerPluginProduct(){}
    virtual ~StirerPluginProduct(){}

    virtual void stir(double intensity) = 0;
};

#endif // STIRERPLUGINPRODUCT_H
