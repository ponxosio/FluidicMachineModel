#ifndef VAIABLEDOMAIN_H
#define VAIABLEDOMAIN_H

#include <vector>
#include <memory>

#include "rule.h"
#include "arithmetic/variable.h"
#include "arithmetic/integernumber.h"

#include "fluidicmachinemodel_global.h"

class VARIABLEDOMAIN_EXPORT VariableDomain : public Rule
{
    typedef struct
    {
        inline bool operator() (const std::tuple<std::shared_ptr<IntegerNumber>, std::shared_ptr<IntegerNumber>> & tuple1,
                                const std::tuple<std::shared_ptr<IntegerNumber>, std::shared_ptr<IntegerNumber>> & tuple2) const throw()
        {
            std::shared_ptr<IntegerNumber> number1 = std::get<0>(tuple1);
            std::shared_ptr<IntegerNumber> number2 = std::get<0>(tuple2);
            return number2->getValue() < number1->getValue();
        }
    } DomainTupleComparator;

public:
    typedef std::tuple<std::shared_ptr<IntegerNumber>, std::shared_ptr<IntegerNumber>> DomainTuple;

    VariableDomain(std::shared_ptr<Variable> variable,
                   const std::vector<DomainTuple> & domain);
    virtual ~VariableDomain();

    virtual void fillTranslationStack(TranslationStack* stack);

protected:
    std::shared_ptr<Variable> variable;
    std::vector<DomainTuple> domain;
};

#endif // VAIABLEDOMAIN_H
