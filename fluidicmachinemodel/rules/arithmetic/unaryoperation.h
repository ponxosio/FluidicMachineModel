#ifndef UNARYOPERATOR_H
#define UNARYOPERATOR_H

#include <string>
#include <memory>

#include "fluidicmachinemodel/rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class FLUIDICMODEL_UNARYOPERATION_EXPORT UnaryOperation : public ArithmeticOperable
{
public:
    typedef enum UnaryOperators_ {
        absolute_value,
    } UnaryOperators;

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
