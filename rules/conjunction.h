#ifndef CONJUNCTION_H
#define CONJUNCTION_H

#include <memory>

#include "rules/predicate.h"

#include "fluidicmachinemodel_global.h"

typedef enum BoolOperators_ {
    conjuntion,
    disjunction
} BoolOperators;

class CONJUCTION_EXPORT Conjunction : public Predicate
{
public:
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
