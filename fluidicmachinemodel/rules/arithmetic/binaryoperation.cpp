#include "binaryoperation.h"

BinaryOperation::BinaryOperation(std::shared_ptr<ArithmeticOperable> left, BinaryOperators op, std::shared_ptr<ArithmeticOperable> right) :
    ArithmeticOperable(), left(left), right(right)
{
    this->op = op;
}

BinaryOperation::~BinaryOperation() {

}

void BinaryOperation::fillTranslationStack(TranslationStack* stack) {
    left->fillTranslationStack(stack);
    right->fillTranslationStack(stack);
    stack->stackArithmeticBinaryOperation(op);
}
