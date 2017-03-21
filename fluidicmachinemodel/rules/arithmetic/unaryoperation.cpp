#include "unaryoperation.h"

UnaryOperation::UnaryOperation(std::shared_ptr<ArithmeticOperable> operable, UnaryOperators op) :
    ArithmeticOperable()
{
    this->operable = operable;
    this->op = op;
}

UnaryOperation::~UnaryOperation() {

}

void UnaryOperation::fillTranslationStack(TranslationStack* stack) {
    operable->fillTranslationStack(stack);
    stack->stackArithmeticUnaryOperation(op);
}
