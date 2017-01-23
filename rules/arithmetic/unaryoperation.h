#ifndef UNARYOPERATOR_H
#define UNARYOPERATOR_H

#include <string>
#include <memory>

#include "rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel_global.h"

typedef enum UnaryOperators_ {
    absolute_value,
} UnaryOperators;

class UnaryOperation : public ArithmeticOperable
{
public:
    UnaryOperation(std::shared_ptr<ArithmeticOperable> operable, UnaryOperators op);
    virtual ~UnaryOperation();

    virtual void fillTranslationStack(TranslationStack* stack);

    inline std::shared_ptr<ArithmeticOperable> getOperable() {
        return operable;
    }

    inline UnaryOperators getOperator() {
        return op;
    }
protected:
    std::shared_ptr<ArithmeticOperable> operable;
    UnaryOperators op;
};

#endif // UNARYOPERATOR_H
