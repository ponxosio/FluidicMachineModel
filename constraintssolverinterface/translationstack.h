#ifndef TRANSLATIONSTACK_H
#define TRANSLATIONSTACK_H

# include <string>

#include "constraintssolverinterface/routingengine.h"

class TranslationStack {
public:
    TranslationStack() {}
    virtual ~TranslationStack(){}

    virtual RoutingEngine* getRoutingEngine() = 0;

    virtual void pop() = 0;
    virtual void clear() = 0;
    virtual void addHeadToRestrictions() = 0;
    virtual void stackVariable(const std::string & name) = 0;
    virtual void stackNumber(int value) = 0;
    virtual void stackArithmeticBinaryOperation(int arithmeticOp) = 0;
    virtual void stackEquality() = 0;
    virtual void stackBooleanConjuction(int booleanOp) = 0;
    virtual void stackImplication() = 0;
};

#endif // TRANSLATIONSTACK_H
