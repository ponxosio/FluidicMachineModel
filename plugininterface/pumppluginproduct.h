#ifndef PUMPPLUGINPRODUCT_H
#define PUMPPLUGINPRODUCT_H

class PumpPluginProduct {
public:
    PumpPluginProduct(){}
    virtual ~PumpPluginProduct(){}

    virtual void setPumpState(int dir, double rate) = 0;
};

#endif // PUMPPLUGINPRODUCT_H
