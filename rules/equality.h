#ifndef EQUALITY_H
#define EQUALITY_H

#include <memory>

#include "rules/predicate.h"
#include "rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel_global.h"

class EQUALITY_EXPORT Equality : public Predicate
{
public:
    Equality(std::shared_ptr<ArithmeticOperable> left, std::shared_ptr<ArithmeticOperable> rigth);
    virtual ~Equality();

    virtual void fillTranslationStack(TranslationStack* stack);

    inline std::shared_ptr<ArithmeticOperable> getLeftOperation() {
        return left;
    }

    inline std::shared_ptr<ArithmeticOperable> getRightOperation() {
        return rigth;
    }

protected:
    std::shared_ptr<ArithmeticOperable> left;
    std::shared_ptr<ArithmeticOperable> rigth;
};

#endif // EQUALITY_H
