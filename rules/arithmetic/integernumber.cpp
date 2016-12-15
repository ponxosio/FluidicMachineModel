#include "integernumber.h"

IntegerNumber::IntegerNumber(int value) :
    ArithmeticOperable()
{
    this->value = value;
}

IntegerNumber::~IntegerNumber() {

}

void IntegerNumber::fillTranslationStack(TranslationStack* stack) {
    stack->stackNumber(value);
}
