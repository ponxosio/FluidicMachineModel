#include "variabledomain.h"

VariableDomain::VariableDomain(std::shared_ptr<Variable> variable,
                               const std::vector<DomainTuple> & domain) :
    Rule(), domain(domain)
{
    this->variable = variable;
}

VariableDomain::~VariableDomain() {

}

void VariableDomain::fillTranslationStack(TranslationStack* stack) {
    for (std::tuple<std::shared_ptr<IntegerNumber>,std::shared_ptr<IntegerNumber>> tuple: domain) {
        std::get<0>(tuple)->fillTranslationStack(stack);
        std::get<1>(tuple)->fillTranslationStack(stack);
    }
    variable->fillTranslationStack(stack);
    stack->stackVarDomain();
}
