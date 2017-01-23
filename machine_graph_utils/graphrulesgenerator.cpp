#include "graphrulesgenerator.h"

const std::shared_ptr<IntegerNumber> GraphRulesGenerator::minusOne = std::make_shared<IntegerNumber>(-1);
const std::shared_ptr<IntegerNumber> GraphRulesGenerator::zero = std::make_shared<IntegerNumber>(0);
const std::shared_ptr<IntegerNumber> GraphRulesGenerator::one = std::make_shared<IntegerNumber>(1);

GraphRulesGenerator::GraphRulesGenerator(GraphPtr graph,
                    const std::vector<int> * pumps,
                    const std::vector<int> * valves,
                    const std::vector<int> * openContainers,
                    const std::vector<int> * closeContainers,
                    const std::vector<std::tuple<int,int>>* tubes,
                    const std::unordered_map<short int,short int> * idOpenContainerLiquidIdMap,
                    short int ratePrecisionInteger,
                    short int ratePrecisionDecimal)
    throw(std::invalid_argument)
{
    this->graph = graph;
    this->pumps = pumps;
    this->valves = valves;
    this->openContainers = openContainers;
    this->closeContainers = closeContainers;
    this->tubes = tubes;
    this->ratePrecisionDecimal = ratePrecisionDecimal;
    this->ratePrecisionInteger = ratePrecisionInteger;
    this->idOpenContainerLiquidIdMap = idOpenContainerLiquidIdMap;

    this->powerValue = (long long) pow(10, ratePrecisionDecimal + ratePrecisionInteger);
    this->maxRateValue = (long long) (pow(10, ratePrecisionDecimal + ratePrecisionInteger) - 1);
    this->power = std::make_shared<IntegerNumber>(powerValue);
    this->maxRate = std::make_shared<IntegerNumber>(maxRateValue);

    generateDomain();
    std::shared_ptr<Predicate> statesPredicate = generatePumpsCloseState();
}

GraphRulesGenerator::~GraphRulesGenerator() {

}

std::shared_ptr<Predicate> GraphRulesGenerator::generateEqualityContainer(GraphPtr graph, int id) throw(std::invalid_argument) {
    if (graph->getNode(id)) {
        MachineGraph::EdgeVectorPtr arrivingTubes = graph->getArrivingEdges(id);
        MachineGraph::EdgeVectorPtr leavingTubes = graph->getLeavingEdges(id);

        if(arrivingTubes->empty() && leavingTubes->empty()) {
            throw(std::invalid_argument(std::to_string(id) + " id, is an isolated node"));
        }
        else if (arrivingTubes->empty())  //only leaving tube
        {
            std::shared_ptr<ArithmeticOperable> sumLeaving = addVariables(generateVariablesVector(leavingTubes));
            sumLeaving = std::make_shared<BinaryOperation>(getNumber(-1), BinaryOperation::multiply, sumLeaving);
            return createEquality(makeContainerName(id), sumLeaving);
        }
        else //arriving tubes alone or with leaving tubes
        {
            std::shared_ptr<ArithmeticOperable> sumArriving = addVariables(generateVariablesVector(arrivingTubes));
            std::shared_ptr<Predicate> mainPredicate=  createEquality(makeContainerName(id), sumArriving);

            for(auto it = leavingTubes->begin(); it != leavingTubes->end(); ++it) {
                MachineGraph::EdgeTypePtr edge = *it;
                std::shared_ptr<Predicate> tempEq = createEquality(makeTubeName(edge->getIdSource(), edge->getIdTarget()), sumArriving);
                mainPredicate = andPredicates(mainPredicate, tempEq);
            }
            return mainPredicate;
        }
    } else {
        throw(std::invalid_argument(std::to_string(id) + " id, does not exists on the graph"));
    }
}

std::vector<std::shared_ptr<Variable>> GraphRulesGenerator::generateVariablesVector(MachineGraph::EdgeVectorPtr edges)
{
    std::vector<std::shared_ptr<Variable>> variables;
    for(auto it = edges->begin(); it != edges->end(); ++it) {
        MachineGraph::EdgeTypePtr edge = *it;
        variables.push_back(getVariable(makeTubeName(edge->getIdSource(), edge->getIdTarget())));
    }
    return variables;
}

std::shared_ptr<Predicate> GraphRulesGenerator::generatePumpsCloseState() {
    std::shared_ptr<Predicate> mainPred = createEquality(makePumpDirName(pumps->front()), 0);
    mainPred = andPredicates(mainPred, createEquality(makePumpRateName(pumps->front()), 0));

    for(auto it = pumps->begin() + 1; it != pumps->end(); ++it) {
        std::shared_ptr<Predicate> tempPred = createEquality(makePumpDirName(*it), 0);
        mainPred = andPredicates(mainPred, tempPred);

        tempPred = createEquality(makePumpRateName(*it), 0);
        mainPred = andPredicates(mainPred, tempPred);
    }

    for(auto it = valves->begin(); it != valves->end(); ++it) {
        std::string name = makeValveName(*it);

        std::vector<int> positions = getValvePossibleValues(*it);
        std::shared_ptr<Predicate> tempPred = createEquality(name, *positions.begin());
        for (auto itPos = positions.begin() + 1; itPos < positions.end(); ++itPos) {
            std::shared_ptr<Predicate> actualPosEq = createEquality(name, *itPos);
            tempPred = orPredicates(tempPred, actualPosEq);
        }
        mainPred = andPredicates(mainPred, tempPred);
    }

    for(auto it = closeContainers->begin(); it != closeContainers->end(); ++it) {
        std::shared_ptr<Predicate> tempPred = createEquality(makeContainerName(*it), 0);
        mainPred = andPredicates(mainPred, tempPred);
    }

    for(auto it = tubes->begin(); it != tubes->end(); ++it) {
        std::shared_ptr<Predicate> tempPred = createEquality(makeTubeName(*it), 0);
        mainPred = andPredicates(mainPred, tempPred);
    }

    return mainPred;
}

void GraphRulesGenerator::generateDomain() {
    for (int pump: *pumps) {
        std::shared_ptr<VariableDomain> domainRuleDir = generateDomainPumpDir(pump);
        rules.push_back(domainRuleDir);

        std::shared_ptr<VariableDomain> domainRuleRate = generateDomainPumpRate(pump);
        rules.push_back(domainRuleRate);
    }

    for (int valve: *valves) {
         std::shared_ptr<VariableDomain> domainValve = generateDomainValve(valve);
         rules.push_back(domainValve);
    }

    for (int openCnt: *openContainers) {
        std::shared_ptr<VariableDomain> domainValue = generateDomainOpenContainer(openCnt);
        rules.push_back(domainValue);
    }

    for (int closeCnt: *closeContainers) {
        std::shared_ptr<VariableDomain> domainValue = generateDomainCloseContainer(closeCnt);
        rules.push_back(domainValue);
    }

    for (std::tuple<int,int> tube: *tubes) {
        std::shared_ptr<VariableDomain> domainValue = generateDomainTube(tube);
        rules.push_back(domainValue);
    }
}

std::shared_ptr<VariableDomain> GraphRulesGenerator::generateDomainValve(int id) {
    std::shared_ptr<Variable> var = getVariable(makeValveName(id));

    std::vector<long long> possiblePosition;
    for(int value: getValvePossibleValues(id)) {
        possiblePosition.push_back(value);
    }
    std::vector<VariableDomain::DomainTuple> valveDomain = generateDomainForValues(possiblePosition);

    return std::make_shared<VariableDomain>(var, valveDomain);
}

std::vector<int> GraphRulesGenerator::getValvePossibleValues(int id) {
    std::shared_ptr<ValveNode> valvePtr = std::dynamic_pointer_cast<ValveNode>(graph->getNode(id));
    return valvePtr->getValvePossibleValues();
}

std::vector<VariableDomain::DomainTuple> GraphRulesGenerator::generateDomainForValues(const std::vector<long long> & values)
{
    std::vector<VariableDomain::DomainTuple> domain;
    if (values.size() > 1) {
        int iniValue = values.front();
        int lastValue = -1;
        for (auto it = (values.begin() + 1) ; it != values.end(); ++it) {
            lastValue = *it;
            if (lastValue != (iniValue + 1)) {
                domain.push_back(std::make_tuple(getNumber(iniValue),
                                                 getNumber(lastValue)));
                iniValue = lastValue;
                lastValue = -1;
            }
        }
        if (lastValue == -1) {
            domain.push_back(std::make_tuple(getNumber(iniValue),
                                             getNumber(lastValue)));
        }
    }
    return domain;
}

std::shared_ptr<VariableDomain> GraphRulesGenerator::generateDomainPumpDir(int id) {
    std::vector<VariableDomain::DomainTuple> domain;

    std::shared_ptr<Variable> var = getVariable(makePumpDirName(id));
    VariableDomain::DomainTuple tuple = std::make_tuple(std::make_shared<IntegerNumber>(-1),std::make_shared<IntegerNumber>(1));
    domain.push_back(tuple);

    return std::make_shared<VariableDomain>(var, domain);
}

std::shared_ptr<VariableDomain> GraphRulesGenerator::generateDomainPumpRate(int id) {
    std::vector<VariableDomain::DomainTuple> domain;

    std::shared_ptr<Variable> var = getVariable(makePumpRateName(id));
    VariableDomain::DomainTuple tuple = std::make_tuple(std::make_shared<IntegerNumber>(0), maxRate);
    domain.push_back(tuple);

    return std::make_shared<VariableDomain>(var, domain);
}

std::shared_ptr<VariableDomain> GraphRulesGenerator::generateDomainCloseContainer(int id) {
    int maxLiquidIdValue = (1 << (openContainers->size()));
    std::vector<long long> possibleValues;
    possibleValues.push_back(0);

    for(int i = 1; i < maxLiquidIdValue; i++) {
        possibleValues.push_back(i*powerValue + 1);
        possibleValues.push_back(i*powerValue + (powerValue - 1));
    }
    std::sort(possibleValues.begin(), possibleValues.end());

    std::shared_ptr<Variable> var = getVariable(makeContainerName(id));
    std::vector<VariableDomain::DomainTuple> domain = generateDomainForValues(possibleValues);

    return std::make_shared<VariableDomain>(var, domain);
}

std::shared_ptr<VariableDomain> GraphRulesGenerator::generateDomainTube(const std::tuple<int,int> & tuple) {
    int maxLiquidIdValue = (1 << (openContainers->size()));
    std::vector<long long> possibleValues;
    possibleValues.push_back(0);

    for(int i = 1; i < maxLiquidIdValue; i++) {
        possibleValues.push_back(i*powerValue + 1);
        possibleValues.push_back(i*powerValue + (powerValue - 1));
        possibleValues.push_back(-i*powerValue - 1);
        possibleValues.push_back(-i*powerValue - (powerValue - 1));
    }
    std::sort(possibleValues.begin(), possibleValues.end());

    std::shared_ptr<Variable> var = getVariable(makeTubeName(tuple));
    std::vector<VariableDomain::DomainTuple> domain = generateDomainForValues(possibleValues);

    return std::make_shared<VariableDomain>(var, domain);
}

std::shared_ptr<VariableDomain> GraphRulesGenerator::generateDomainOpenContainer(int id) throw(std::invalid_argument) {
    auto it = idOpenContainerLiquidIdMap->find(id);
    if (it != idOpenContainerLiquidIdMap->end()) {
        std::vector<long long> possibleValues;
        int maxLiquidIdValue = (1 << (openContainers->size()));

        long long myLiquidId = ((long long)1) << (it->second);
        possibleValues.push_back(0);
        possibleValues.push_back(myLiquidId*powerValue + 1);
        possibleValues.push_back(myLiquidId*powerValue + (powerValue - 1));
        possibleValues.push_back(-myLiquidId*powerValue - 1);
        possibleValues.push_back(-myLiquidId*powerValue - (powerValue - 1));

        int actual2power = 4;
        for(int i = 3; i < maxLiquidIdValue; i++) {
            if ((actual2power % i) == 0) {
                actual2power *= 2;
            } else {
                possibleValues.push_back(i*powerValue + 1);
                possibleValues.push_back(i*powerValue + (powerValue - 1));
                possibleValues.push_back(-i*powerValue - 1);
                possibleValues.push_back(-i*powerValue - (powerValue - 1));
            }
        }
        std::sort(possibleValues.begin(), possibleValues.end());

        std::shared_ptr<Variable> var = getVariable(makeContainerName(id));
        std::vector<VariableDomain::DomainTuple> domain = generateDomainForValues(possibleValues);

        return std::make_shared<VariableDomain>(var, domain);
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not found on the open containers map"));
    }
}

std::shared_ptr<ArithmeticOperable> GraphRulesGenerator::addVariables(std::shared_ptr<Variable> var1, std::shared_ptr<Variable> var2) {
    std::shared_ptr<ArithmeticOperable> rate = calculateRate(var1);
    std::shared_ptr<ArithmeticOperable> main = std::make_shared<BinaryOperation>(calculateId(var1), BinaryOperation::add, calculateId(var2));
    return std::make_shared<BinaryOperation>(main, BinaryOperation::add, rate);
}

std::shared_ptr<ArithmeticOperable> GraphRulesGenerator::addVariables(const std::vector<std::shared_ptr<Variable>> & variables)
    throw(std::invalid_argument)
{
    if (variables.size() > 1) {
        std::shared_ptr<ArithmeticOperable> rate = calculateRate(variables.front());

        std::shared_ptr<ArithmeticOperable> main = calculateId(variables.front());
        for (auto it = variables.begin(); it != variables.end(); ++it) {
            std::shared_ptr<ArithmeticOperable> temp = calculateId(*it);
            main = std::make_shared<BinaryOperation>(main, BinaryOperation::add, temp);
        }

        return std::make_shared<BinaryOperation>(main, BinaryOperation::add, rate);
    } else {
        throw(std::invalid_argument("the method must received at least 2 variable to be added"));
    }
}

std::shared_ptr<ArithmeticOperable> GraphRulesGenerator::calculateRate(std::shared_ptr<Variable> variable) {
    std::shared_ptr<ArithmeticOperable> absVar = std::make_shared<UnaryOperation>(variable, absolute_value);
    std::shared_ptr<ArithmeticOperable> powerNum = std::make_shared<IntegerNumber>(powerValue);
    return std::make_shared<BinaryOperation>(absVar, BinaryOperation::module, powerNum);
}

std::shared_ptr<ArithmeticOperable> GraphRulesGenerator::calculateId(std::shared_ptr<Variable> variable) {
    std::shared_ptr<ArithmeticOperable> absVar = std::make_shared<UnaryOperation>(variable, absolute_value);
    std::shared_ptr<ArithmeticOperable> division = std::make_shared<BinaryOperation>(absVar, BinaryOperation::divide, power);
    return std::make_shared<BinaryOperation>(division, BinaryOperation::multiply, power);
}

std::shared_ptr<Variable> GraphRulesGenerator::getVariable(const std::string & name) {
    auto it = variableMap.find(name);
    if (it != variableMap.end()) {
        return it->second;
    } else {
        std::shared_ptr<Variable> var = std::make_shared<Variable>(name);
        variableMap.insert(std::make_pair(std::string(name), var));
        return var;
    }
}

std::shared_ptr<IntegerNumber> GraphRulesGenerator::getNumber(long long value) {
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
