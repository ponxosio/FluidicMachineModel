#include "variable.h"

Variable::Variable(const std::string & name) :
    ArithmeticOperable()
{
    this->name = std::string(name);
}

Variable::~Variable() {

}

void Variable::fillTranslationStack(TranslationStack* stack) {
    stack->stackVariable(name);
}
