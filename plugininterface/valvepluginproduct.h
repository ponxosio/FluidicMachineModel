#ifndef VALVEPLUGINPRODUCT_H
#define VALVEPLUGINPRODUCT_H

class ValvePluginProduct {
public:
    ValvePluginProduct(){}
    virtual ~ValvePluginProduct(){}

    virtual void moveToPosition(int position) = 0;
};

#endif // VALVEPLUGINPRODUCT_H
