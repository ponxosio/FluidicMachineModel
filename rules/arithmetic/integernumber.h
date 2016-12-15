#ifndef INTEGERNUMBER_H
#define INTEGERNUMBER_H

#include "rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel_global.h"

class INTEGERNUMBER_EXPORT IntegerNumber : public ArithmeticOperable
{
public:
    IntegerNumber(int value);
    virtual ~IntegerNumber();

    virtual void fillTranslationStack(TranslationStack* stack);

    inline int getValue() {
        return value;
    }

protected:
    int value;
};

#endif // INTEGERNUMBER_H
