#ifndef COMMOMRULESOPERATIONS_H
#define COMMOMRULESOPERATIONS_H

#include <memory>

#include "rules/rule.h"
#include "rules/variabledomain.h"
#include "rules/predicate.h"
#include "rules/equality.h"
#include "rules/conjunction.h"
#include "rules/arithmetic/arithmeticoperable.h"
#include "rules/arithmetic/binaryoperation.h"
#include "rules/arithmetic/integernumber.h"
#include "rules/arithmetic/unaryoperation.h"
#include "rules/arithmetic/variable.h"

class DomainGenerator;
class GraphRulesGenerator;
class StatePredicateGenerator;

class CommomRulesOperations
{
    friend class DomainGenerator;
    friend class GraphRulesGenerator;
    friend class StatePredicateGenerator;
    friend class ShortStatePredicateGenerator;

public:
    virtual ~CommomRulesOperations();

protected:
    short int ratePrecisionInteger;
    short int ratePrecisionDecimal;

    long long powerValue;
    long long maxRateValue;
    std::shared_ptr<IntegerNumber> power;
    std::shared_ptr<IntegerNumber> maxRate;
    std::shared_ptr<IntegerNumber> minusOne;
    std::shared_ptr<IntegerNumber> zero;
    std::shared_ptr<IntegerNumber> one;

    std::unordered_map<std::string, std::shared_ptr<Variable>> variableMap;

    CommomRulesOperations(short int ratePrecisionInteger,  short int ratePrecisionDecimal);

    std::shared_ptr<ArithmeticOperable> addVariables(std::shared_ptr<Variable> var1, std::shared_ptr<Variable> var2) throw(std::invalid_argument);
    std::shared_ptr<ArithmeticOperable> addVariables(const std::vector<std::shared_ptr<Variable>> & variables) throw(std::invalid_argument);
    std::shared_ptr<ArithmeticOperable> calculateRate(std::shared_ptr<Variable> variable);
    std::shared_ptr<ArithmeticOperable> calculateId(std::shared_ptr<Variable> variable);

    std::shared_ptr<Variable> getVariable(const std::string & name);
    std::shared_ptr<IntegerNumber> getNumber(long long value);

    std::shared_ptr<ArithmeticOperable> addOperables(std::shared_ptr<ArithmeticOperable> op1, std::shared_ptr<ArithmeticOperable> op2);
    std::shared_ptr<Predicate> andPredicates(std::shared_ptr<Predicate> pred1, std::shared_ptr<Predicate> pred2);
    std::shared_ptr<Predicate> orPredicates(std::shared_ptr<Predicate> pred1, std::shared_ptr<Predicate> pred2);

    inline std::shared_ptr<Predicate> createEquality(const std::string & name, int value) {
        return std::make_shared<Equality>(getVariable(name),Equality::equal, getNumber(value));
    }
    inline std::shared_ptr<Predicate> createEquality(const std::string & name1, const std::string & name2) {
        return std::make_shared<Equality>(getVariable(name1),Equality::equal, getVariable(name2));
    }
    inline std::shared_ptr<Predicate> createEquality(const std::string & name1, std::shared_ptr<ArithmeticOperable> op) {
        return std::make_shared<Equality>(getVariable(name1),Equality::equal, op);
    }
    inline std::shared_ptr<Predicate> createEquality(std::shared_ptr<ArithmeticOperable> op1, std::shared_ptr<ArithmeticOperable> op2) {
        return std::make_shared<Equality>(op1,Equality::equal, op2);
    }
    inline std::shared_ptr<Predicate> createBiggerEquals(const std::string & name, int value) {
        return std::make_shared<Equality>(getVariable(name), Equality::bigger_equal, getNumber(value));
    }
    inline std::shared_ptr<Predicate> createLesserEquals(const std::string & name, int value) {
        return std::make_shared<Equality>(getVariable(name), Equality::lesser_equal, getNumber(value));
    }
    inline std::shared_ptr<Predicate> createBigger(const std::string & name, int value) {
        return std::make_shared<Equality>(getVariable(name), Equality::bigger, getNumber(value));
    }
    inline std::shared_ptr<Predicate> createBigger(std::shared_ptr<ArithmeticOperable> op1, std::shared_ptr<ArithmeticOperable> op2) {
        return std::make_shared<Equality>(op1, Equality::bigger, op2);
    }
    inline std::shared_ptr<Predicate> createLesser(const std::string & name, int value) {
        return std::make_shared<Equality>(getVariable(name), Equality::lesser, getNumber(value));
    }
    inline std::shared_ptr<Predicate> createLesser(std::shared_ptr<ArithmeticOperable> op1, std::shared_ptr<ArithmeticOperable> op2) {
        return std::make_shared<Equality>(op1, Equality::lesser, op2);
    }
    inline std::shared_ptr<Predicate> createNotEqual(const std::string & name, int value) {
        return std::make_shared<Equality>(getVariable(name), Equality::not_equal, getNumber(value));
    }
    inline std::shared_ptr<Predicate> createNotEqual(std::shared_ptr<ArithmeticOperable> op1, std::shared_ptr<ArithmeticOperable> op2) {
        return std::make_shared<Equality>(op1, Equality::not_equal, op2);
    }
};

#endif // COMMOMRULESOPERATIONS_H
