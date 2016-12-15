#include "conjunction.h"

Conjunction::Conjunction(std::shared_ptr<Predicate> left, BoolOperators op, std::shared_ptr<Predicate> right) :
    Predicate(), left(left), right(right)
{
    this->op = op;
}

Conjunction::~Conjunction() {

}

void Conjunction::fillTranslationStack(TranslationStack* stack) {
    left->fillTranslationStack(stack);
    right->fillTranslationStack(stack);
    stack->stackBooleanConjuction(op);
}

