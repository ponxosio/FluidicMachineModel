#include "implication.h"

Implication::Implication(std::shared_ptr<Predicate> left, std::shared_ptr<Predicate> right) :
    Rule(), left(left), right(right)
{

}

Implication::~Implication()
{

}

void Implication::fillTranslationStack(TranslationStack* stack) {
    left->fillTranslationStack(stack);
    right->fillTranslationStack(stack);
    stack->stackImplication();
}
