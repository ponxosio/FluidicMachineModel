#include "integernumber.h"

IntegerNumber::IntegerNumber(long long value) :
    ArithmeticOperable()
{
    this->value = value;
}

IntegerNumber::~IntegerNumber() {

}

void IntegerNumber::fillTranslationStack(TranslationStack* stack) {
    stack->stackNumber(value);
}
