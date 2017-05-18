#include "unaryoperation.h"

RuleUnaryOperation::RuleUnaryOperation(std::shared_ptr<ArithmeticOperable> operable, UnaryOperators op) :
    ArithmeticOperable()
{
    this->operable = operable;
    this->op = op;
}

RuleUnaryOperation::~RuleUnaryOperation() {

}

void RuleUnaryOperation::fillTranslationStack(TranslationStack* stack) {
    operable->fillTranslationStack(stack);
    stack->stackArithmeticUnaryOperation(op);
}
