#ifndef UNARYOPERATOR_H
#define UNARYOPERATOR_H

#include <string>
#include <memory>

#include "fluidicmachinemodel/rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class FLUIDICMODEL_UNARYOPERATION_EXPORT RuleUnaryOperation : public ArithmeticOperable
{
public:
    typedef enum UnaryOperators_ {
        absolute_value,
    } UnaryOperators;

    RuleUnaryOperation(std::shared_ptr<ArithmeticOperable> operable, UnaryOperators op);
    virtual ~RuleUnaryOperation();

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
