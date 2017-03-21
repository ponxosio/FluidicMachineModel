#ifndef ARITHMETICOPERABLE_H
#define ARITHMETICOPERABLE_H

#include "fluidicmachinemodel/constraintssolverinterface/translationstack.h"

class ArithmeticOperable
{

public:
    ArithmeticOperable(){}
    virtual ~ArithmeticOperable(){}

    virtual void fillTranslationStack(TranslationStack* stack) = 0;
};

#endif // ARITHMETICOPERABLE_H
