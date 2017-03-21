#ifndef CONJUNCTION_H
#define CONJUNCTION_H

#include <memory>

#include "fluidicmachinemodel/rules/predicate.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class CONJUCTION_EXPORT Conjunction : public Predicate
{
public:
    typedef enum BoolOperators_ {
        predicate_and,
        predicate_or
    } BoolOperators;

    Conjunction(std::shared_ptr<Predicate> left, BoolOperators op, std::shared_ptr<Predicate> right);
    virtual ~Conjunction();

    virtual void fillTranslationStack(TranslationStack* stack);

    std::shared_ptr<Predicate> getLeftPredicate() {
        return left;
    }

    std::shared_ptr<Predicate> getRightPredicate() {
        return right;
    }

    inline BoolOperators getOperation() {
        return op;
    }

 protected:
    std::shared_ptr<Predicate> left;
    std::shared_ptr<Predicate> right;
    BoolOperators op;
};

#endif // CONJUNCTION_H
