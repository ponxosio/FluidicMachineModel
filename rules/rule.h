#ifndef RULE_H
#define RULE_H

#include "rules/translationstack.h"

class Rule
{
public:
    Rule(){}
    virtual ~Rule(){}

    virtual void fillTranslationStack(TranslationStack* stack) = 0;
};

#endif // RULE_H
