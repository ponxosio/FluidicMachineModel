#include "domaingenerator.h"
#include "machine_graph_utils/graphrulesgenerator.h"
#include "commomrulesoperations.h"


DomainGenerator::DomainGenerator(std::shared_ptr<MachineGraph> graph, std::shared_ptr<CommomRulesOperations> calculator)
    throw(std::invalid_argument)
{
    this->graph = graph;
    this->calculator = calculator;
    generateDomain();
}

DomainGenerator::~DomainGenerator() {

}

void DomainGenerator::generateDomain() {
    for (int pump: graph->getPumpsIdsSet()) {
        std::shared_ptr<VariableDomain> domainRuleDir = generateDomainPumpDir(pump);
        rules.push_back(domainRuleDir);

        std::shared_ptr<VariableDomain> domainRuleRate = generateDomainPumpRate(pump);
        rules.push_back(domainRuleRate);
    }

    for (int valve: graph->getValvesIdsSet()) {
         std::shared_ptr<VariableDomain> domainValve = generateDomainValve(valve);
         rules.push_back(domainValve);
    }

    for (int openCnt: graph->getOpenContainersIdsSet()) {
        std::shared_ptr<VariableDomain> domainValue = generateDomainOpenContainer(openCnt);
        rules.push_back(domainValue);
    }

    for (int closeCnt: graph->getCloseContainersIdsSet()) {
        std::shared_ptr<VariableDomain> domainValue = generateDomainCloseContainer(closeCnt);
        rules.push_back(domainValue);
    }

    for (auto it : *graph->getAlledgesMap().get()) {
        MachineGraph::GraphType::EdgeTypePtr edge = it.second;
        std::shared_ptr<VariableDomain> domainValue = generateDomainTube(edge->getIdSource(), edge->getIdTarget());
        rules.push_back(domainValue);
    }
}

std::shared_ptr<VariableDomain> DomainGenerator::generateDomainValve(int id) throw(std::invalid_argument) {
    std::vector<VariableDomain::DomainTuple> valveDomain;
    std::shared_ptr<Variable> var = calculator->getVariable(VariableNominator::getValveVarName(id));

    std::set<int> possiblePositions = getValvePossibleValues(id);

    auto it = possiblePositions.begin();
    int initValue = *it;
    int lastValue = initValue;
    ++it;

    for(; it != possiblePositions.end(); ++it) {
        int actual = *it;
        if (actual > lastValue + 1) {
            valveDomain.push_back(std::make_tuple(calculator->getNumber(initValue),
                                                  calculator->getNumber(lastValue)));
            initValue = actual;
        }
        lastValue = actual;
    }
    valveDomain.push_back(std::make_tuple(calculator->getNumber(initValue),
                                          calculator->getNumber(lastValue)));

    return std::make_shared<VariableDomain>(var, valveDomain);
}

std::set<int> DomainGenerator::getValvePossibleValues(int id) throw(std::invalid_argument) {
    try {
        std::shared_ptr<ValveNode> valvePtr = graph->getValve(id);
        return valvePtr->getValvePossibleValues();
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("impossible to get valve's values. Error: " + std::string(e.what())));
    }
}

std::vector<VariableDomain::DomainTuple> DomainGenerator::generateDomainForValues(const std::vector<long long> & values) throw(std::invalid_argument)
{
    std::vector<VariableDomain::DomainTuple> domain;
    if (!values.empty()) {
        if (values.size() == 1) {
            int iniValue = values.back();
            domain.push_back(std::make_tuple(calculator->getNumber(iniValue),
                                             calculator->getNumber(iniValue)));
        } else if (values.size() % 2 == 0) { //even
            for(int i = 0; i < (values.size() - 1); i+=2) {
                int iniValue = values[i];
                int endValue = values[i + 1];
                domain.push_back(std::make_tuple(calculator->getNumber(iniValue),
                                                 calculator->getNumber(endValue)));
            }
        } else { //uneven
            int middle = (int) std::floor(values.size()/2);

            for(int i = 0; i < (middle-1); i+=2) {
                int iniValue = values[i];
                int endValue = values[i + 1];
                domain.push_back(std::make_tuple(calculator->getNumber(iniValue),
                                                 calculator->getNumber(endValue)));
            }

            domain.push_back(std::make_tuple(calculator->getNumber(values[middle]),
                                             calculator->getNumber(values[middle])));

            for(int i = middle + 1; i < (values.size()-1); i+= 2){
                int iniValue = values[i];
                int endValue = values[i + 1];
                domain.push_back(std::make_tuple(calculator->getNumber(iniValue),
                                                 calculator->getNumber(endValue)));
            }

        }
    } else {
        throw(std::invalid_argument("values vector is empty"));
    }
    return domain;
}

std::shared_ptr<VariableDomain> DomainGenerator::generateDomainPumpDir(int id) {
    std::vector<VariableDomain::DomainTuple> domain;

    std::shared_ptr<Variable> var = calculator->getVariable(VariableNominator::getPumpVarName(id));
    VariableDomain::DomainTuple tuple = std::make_tuple(calculator->getNumber(-1),std::make_shared<IntegerNumber>(1));
    domain.push_back(tuple);

    return std::make_shared<VariableDomain>(var, domain);
}

std::shared_ptr<VariableDomain> DomainGenerator::generateDomainPumpRate(int id) {
    std::vector<VariableDomain::DomainTuple> domain;

    std::shared_ptr<Variable> var = calculator->getVariable(VariableNominator::getPumpRateVarName(id));
    VariableDomain::DomainTuple tuple = std::make_tuple(calculator->getNumber(0), calculator->maxRate);
    domain.push_back(tuple);

    return std::make_shared<VariableDomain>(var, domain);
}

std::shared_ptr<VariableDomain> DomainGenerator::generateDomainCloseContainer(int id) {
    int maxLiquidIdValue = (1 << (graph->getNumOpenContainers()));
    std::vector<long long> possibleValues;
    possibleValues.push_back(0);

    for(int i = 1; i < maxLiquidIdValue; i++) {
        possibleValues.push_back(i*calculator->powerValue + 1);
        possibleValues.push_back(i*calculator->powerValue + (calculator->powerValue - 1));
        possibleValues.push_back(-i*calculator->powerValue - 1);
        possibleValues.push_back(-i*calculator->powerValue - (calculator->powerValue - 1));
    }
    std::sort(possibleValues.begin(), possibleValues.end());

    std::shared_ptr<Variable> var = calculator->getVariable(VariableNominator::getContainerVarName(id));
    std::vector<VariableDomain::DomainTuple> domain = generateDomainForValues(possibleValues);

    return std::make_shared<VariableDomain>(var, domain);
}

std::shared_ptr<VariableDomain> DomainGenerator::generateDomainTube(int idSource, int idTarget) {
    int maxLiquidIdValue = (1 << (graph->getNumOpenContainers()));
    std::vector<long long> possibleValues;
    possibleValues.push_back(0);

    for(int i = 1; i < maxLiquidIdValue; i++) {
        possibleValues.push_back(i*calculator->powerValue + 1);
        possibleValues.push_back(i*calculator->powerValue + (calculator->powerValue - 1));
        possibleValues.push_back(-i*calculator->powerValue - 1);
        possibleValues.push_back(-i*calculator->powerValue - (calculator->powerValue - 1));
    }
    std::sort(possibleValues.begin(), possibleValues.end());

    std::shared_ptr<Variable> var = calculator->getVariable(VariableNominator::getTubeVarName(idSource, idTarget));
    std::vector<VariableDomain::DomainTuple> domain = generateDomainForValues(possibleValues);

    return std::make_shared<VariableDomain>(var, domain);
}

std::shared_ptr<VariableDomain> DomainGenerator::generateDomainOpenContainer(int id) throw(std::invalid_argument) {
    /*auto it = graph->getOpenContainerLiquidIdMap().find(id);
    if (it != graph->getOpenContainerLiquidIdMap().end()) {
        std::vector<long long> possibleValues;
        int maxLiquidIdValue = (1 << (graph->getNumOpenContainers()));

        long long myLiquidId = ((long long)1) << (it->second);
        possibleValues.push_back(0);
        possibleValues.push_back(myLiquidId*calculator->powerValue + 1);
        possibleValues.push_back(myLiquidId*calculator->powerValue + (calculator->powerValue - 1));
        possibleValues.push_back(-myLiquidId*calculator->powerValue - 1);
        possibleValues.push_back(-myLiquidId*calculator->powerValue - (calculator->powerValue - 1));

        int actual2power = 4;
        for(int i = 3; i < maxLiquidIdValue; i++) {
            if ((actual2power % i) == 0) {
                actual2power *= 2;
            } else {
                possibleValues.push_back(i*calculator->powerValue + 1);
                possibleValues.push_back(i*calculator->powerValue + (calculator->powerValue - 1));
                possibleValues.push_back(-i*calculator->powerValue - 1);
                possibleValues.push_back(-i*calculator->powerValue - (calculator->powerValue - 1));
            }
        }
        std::sort(possibleValues.begin(), possibleValues.end());*/

    int maxLiquidIdValue = (1 << (graph->getNumOpenContainers()));
    std::vector<long long> possibleValues;
    possibleValues.push_back(0);

    for(int i = 1; i < maxLiquidIdValue; i++) {
        possibleValues.push_back(i*calculator->powerValue + 1);
        possibleValues.push_back(i*calculator->powerValue + (calculator->powerValue - 1));
        possibleValues.push_back(-i*calculator->powerValue - 1);
        possibleValues.push_back(-i*calculator->powerValue - (calculator->powerValue - 1));
    }
    std::sort(possibleValues.begin(), possibleValues.end());

    std::shared_ptr<Variable> var = calculator->getVariable(VariableNominator::getContainerVarName(id));
    std::vector<VariableDomain::DomainTuple> domain = generateDomainForValues(possibleValues);

    return std::make_shared<VariableDomain>(var, domain);
    /*} else {
    throw(std::invalid_argument(std::to_string(id) + " is not found on the open containers map"));
    }*/
}
