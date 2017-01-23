#ifndef INTEGERNUMBER_H
#define INTEGERNUMBER_H

#include "rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel_global.h"

class INTEGERNUMBER_EXPORT IntegerNumber : public ArithmeticOperable
{
public:
    IntegerNumber(long long value);
    virtual ~IntegerNumber();

    virtual void fillTranslationStack(TranslationStack* stack);

    inline long long getValue() {
        return value;
    }

protected:
    long long value;
};

#endif // INTEGERNUMBER_H
