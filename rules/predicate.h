#ifndef PREDICATE_H
#define PREDICATE_H

#include "rules/rule.h"

class Predicate : public Rule {
public:
    Predicate() {}
    virtual ~Predicate(){}

    virtual void fillTranslationStack(TranslationStack* stack) = 0;
};

#endif // PREDICATE_H
