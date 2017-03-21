#ifndef EQUALITY_H
#define EQUALITY_H

#include <memory>

#include "fluidicmachinemodel/rules/predicate.h"
#include "fluidicmachinemodel/rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class EQUALITY_EXPORT Equality : public Predicate
{
public:
    typedef enum ComparatorOp_ {
        not_equal,
        equal,
        bigger,
        bigger_equal,
        lesser,
        lesser_equal
    }ComparatorOp;

    Equality(std::shared_ptr<ArithmeticOperable> left, ComparatorOp op, std::shared_ptr<ArithmeticOperable> rigth);
    virtual ~Equality();

    virtual void fillTranslationStack(TranslationStack* stack);

    inline std::shared_ptr<ArithmeticOperable> getLeftOperation() {
        return left;
    }

    inline std::shared_ptr<ArithmeticOperable> getRightOperation() {
        return rigth;
    }

protected:
    ComparatorOp op;
    std::shared_ptr<ArithmeticOperable> left;
    std::shared_ptr<ArithmeticOperable> rigth;
};

#endif // EQUALITY_H
