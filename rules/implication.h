#ifndef IMPLICATION_H
#define IMPLICATION_H

#include <memory>

#include "rules/rule.h"
#include "rules/predicate.h"

#include "fluidicmachinemodel_global.h"

class IMPLICATION_EXPORT Implication : public Rule
{
public:
    Implication(std::shared_ptr<Predicate> left, std::shared_ptr<Predicate> right);
    virtual ~Implication();

    virtual void fillTranslationStack(TranslationStack* stack);

    inline std::shared_ptr<Predicate> getLeftPredicate() {
        return left;
    }

    inline std::shared_ptr<Predicate> getRightPredicate() {
        return right;
    }

protected:
    std::shared_ptr<Predicate> left;
    std::shared_ptr<Predicate> right;
};

#endif // IMPLICATION_H
