#ifndef BINARYOPERATION_H
#define BINARYOPERATION_H

#include <string>
#include <memory>

#include "rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel_global.h"

typedef enum BinaryOperators_ {
    add,
    subtract,
    multiply,
    divide
} BinaryOperators;

class BINARYOPERATION_EXPORT BinaryOperation : public ArithmeticOperable
{
public:
    BinaryOperation(std::shared_ptr<ArithmeticOperable> left, BinaryOperators op, std::shared_ptr<ArithmeticOperable> right);
    virtual ~BinaryOperation();

    virtual void fillTranslationStack(TranslationStack* stack);

    inline std::shared_ptr<ArithmeticOperable> getLeft() {
        return left;
    }

    inline std::shared_ptr<ArithmeticOperable> getRight() {
        return right;
    }

    inline BinaryOperators getOperation() {
        return op;
    }

protected:
    std::shared_ptr<ArithmeticOperable> left;
    BinaryOperators op;
    std::shared_ptr<ArithmeticOperable> right;
};


#endif // BINARYOPERATION_H
