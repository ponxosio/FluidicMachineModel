#include "commomrulesoperations.h"
#include "domaingenerator.h"
#include "machine_graph_utils/graphrulesgenerator.h"
#include "machine_graph_utils/rules_generation/statepredicategenerator.h"

CommomRulesOperations::CommomRulesOperations(short int ratePrecisionInteger,  short int ratePrecisionDecimal)
{
    this->ratePrecisionDecimal = ratePrecisionDecimal;
    this->ratePrecisionInteger = ratePrecisionInteger;

    this->powerValue = (long long) pow(10, ratePrecisionDecimal + ratePrecisionInteger);
    this->maxRateValue = (long long) (pow(10, ratePrecisionDecimal + ratePrecisionInteger) - 1);

    this->power = std::make_shared<IntegerNumber>(powerValue);
    this->maxRate = std::make_shared<IntegerNumber>(maxRateValue);
    this->minusOne = std::make_shared<IntegerNumber>(-1);
    this->zero = std::make_shared<IntegerNumber>(0);
    this->one = std::make_shared<IntegerNumber>(1);
}

CommomRulesOperations::~CommomRulesOperations() {
    this->power.reset();
    this->maxRate.reset();
    this->minusOne.reset();
    this->zero.reset();
    this->one.reset();
}

std::shared_ptr<ArithmeticOperable> CommomRulesOperations::addVariables(std::shared_ptr<Variable> var1, std::shared_ptr<Variable> var2) {
    std::shared_ptr<ArithmeticOperable> rate = calculateRate(var1);
    std::shared_ptr<ArithmeticOperable> main = std::make_shared<BinaryOperation>(calculateId(var1), BinaryOperation::add, calculateId(var2));
    return std::make_shared<BinaryOperation>(main, BinaryOperation::add, rate);
}

std::shared_ptr<ArithmeticOperable> CommomRulesOperations::addVariables(const std::vector<std::shared_ptr<Variable>> & variables)
    throw(std::invalid_argument)
{
    if (variables.size() > 1) {
        std::shared_ptr<ArithmeticOperable> rate = calculateRate(variables.front());

        std::shared_ptr<ArithmeticOperable> main = calculateId(variables.front());
        for (auto it = variables.begin() + 1; it != variables.end(); ++it) {
            std::shared_ptr<ArithmeticOperable> temp = calculateId(*it);
            main = std::make_shared<BinaryOperation>(main, BinaryOperation::add, temp);
        }
        return std::make_shared<BinaryOperation>(main, BinaryOperation::add, rate);
    } else {
        throw(std::invalid_argument("the method must received at least 2 variable to be added"));
    }
}

std::shared_ptr<ArithmeticOperable> CommomRulesOperations::calculateRate(std::shared_ptr<Variable> variable) {
    std::shared_ptr<ArithmeticOperable> absVar = std::make_shared<UnaryOperation>(variable, absolute_value);
    std::shared_ptr<ArithmeticOperable> powerNum = std::make_shared<IntegerNumber>(powerValue);
    return std::make_shared<BinaryOperation>(absVar, BinaryOperation::module, powerNum);
}

std::shared_ptr<ArithmeticOperable> CommomRulesOperations::calculateId(std::shared_ptr<Variable> variable) {
    std::shared_ptr<ArithmeticOperable> absVar = std::make_shared<UnaryOperation>(variable, absolute_value);
    std::shared_ptr<ArithmeticOperable> division = std::make_shared<BinaryOperation>(absVar, BinaryOperation::divide, power);
    return std::make_shared<BinaryOperation>(division, BinaryOperation::multiply, power);
}

std::shared_ptr<Variable> CommomRulesOperations::getVariable(const std::string & name) {
    auto it = variableMap.find(name);
    if (it != variableMap.end()) {
        return it->second;
    } else {
        std::shared_ptr<Variable> var = std::make_shared<Variable>(name);
        variableMap.insert(std::make_pair(std::string(name), var));
        return var;
    }
}

std::shared_ptr<IntegerNumber> CommomRulesOperations::getNumber(long long value) {
    std::shared_ptr<IntegerNumber> number;
    if(value == 0) {
        number = zero;
    } else if (value == 1) {
        number = one;
    } else if (value == -1) {
        number = minusOne;
    } else if (value == powerValue) {
        number = power;
    } else if (value == maxRateValue) {
        number = maxRate;
    } else {
        number = std::make_shared<IntegerNumber>(value);
    }
    return number;
}

std::shared_ptr<Predicate> CommomRulesOperations::andPredicates(std::shared_ptr<Predicate> pred1, std::shared_ptr<Predicate> pred2) {
    if (pred1 != NULL && pred2 == NULL) {
        return pred1;
    } else if (pred1 == NULL && pred2 != NULL) {
        return pred2;
    } else if (pred1 != NULL && pred2 != NULL) {
        return std::make_shared<Conjunction>(pred1, Conjunction::predicate_and, pred2);
    } else {
        return NULL;
    }
}

std::shared_ptr<Predicate> CommomRulesOperations::orPredicates(std::shared_ptr<Predicate> pred1, std::shared_ptr<Predicate> pred2) {
    if (pred1 != NULL && pred2 == NULL) {
        return pred1;
    } else if (pred1 == NULL && pred2 != NULL) {
        return pred2;
    } else if (pred1 != NULL && pred2 != NULL) {
        return std::make_shared<Conjunction>(pred1, Conjunction::predicate_or, pred2);
    } else {
        return NULL;
    }
}

std::shared_ptr<ArithmeticOperable> CommomRulesOperations::addOperables(std::shared_ptr<ArithmeticOperable> op1, std::shared_ptr<ArithmeticOperable> op2) {
    if (op1 != NULL && op2 == NULL) {
        return op1;
    } else if (op1 == NULL && op2 != NULL) {
        return op2;
    } else if (op1 != NULL && op2 != NULL) {
        return std::make_shared<BinaryOperation>(op1, BinaryOperation::add, op2);
    } else {
        return NULL;
    }
}
