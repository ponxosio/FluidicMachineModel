#include "equality.h"

Equality::Equality(std::shared_ptr<ArithmeticOperable> left, ComparatorOp op, std::shared_ptr<ArithmeticOperable> rigth):
    Predicate(), left(left), rigth(rigth)
{
    this->op = op;
}

Equality::~Equality(){

}

void Equality::fillTranslationStack(TranslationStack* stack) {
    left->fillTranslationStack(stack);
    rigth->fillTranslationStack(stack);
    stack->stackEquality(op);
}
