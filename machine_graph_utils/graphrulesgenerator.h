#ifndef GRAPHRULESGENERATOR_H
#define GRAPHRULESGENERATOR_H

#define LONG_LONG_DIGITS_SIZE 18

#include <memory>
#include <vector>
#include <unordered_set>
#include <stdexcept>

#include "graph/Graph.h"
#include "fluidicnode/fluidicmachinenode.h"
#include "fluidicnode/pumpnode.h"
#include "fluidicnode/valvenode.h"
#include "fluidicnode/containernode.h"
#include "fluidicedge/tubeedge.h"

#include "rules/arithmetic/arithmeticoperable.h"
#include "rules/rule.h"
#include "rules/variabledomain.h"
#include "rules/predicate.h"
#include "rules/equality.h"
#include "rules/conjunction.h"
#include "rules/arithmetic/variable.h"
#include "rules/arithmetic/integernumber.h"
#include "rules/arithmetic/binaryoperation.h"
#include "rules/arithmetic/unaryoperation.h"

class GraphRulesGenerator
{
    typedef Graph<FluidicMachineNode,TubeEdge> MachineGraph;
    typedef std::shared_ptr<MachineGraph> GraphPtr;

public:
    GraphRulesGenerator(GraphPtr graph,
                        const std::vector<int> * pumps,
                        const std::vector<int> * valves,
                        const std::vector<int> * openContainers,
                        const std::vector<int> * closeContainers,
                        const std::vector<std::tuple<int,int>>* tubes,
                        const std::unordered_map<short int,short int> * idOpenContainerLiquidIdMap,
                        short int ratePrecisionInteger,
                        short int ratePrecisionDecimal) throw(std::invalid_argument);
    virtual ~GraphRulesGenerator();

    inline const std::vector<std::shared_ptr<Rule>> & getRules() {
        return rules;
    }

protected:
    static const std::shared_ptr<IntegerNumber> minusOne;
    static const std::shared_ptr<IntegerNumber> zero;
    static const std::shared_ptr<IntegerNumber> one;

    std::shared_ptr<IntegerNumber> power;
    std::shared_ptr<IntegerNumber> maxRate;
    long long powerValue;
    long long maxRateValue;

    short int ratePrecisionInteger;
    short int ratePrecisionDecimal;

    GraphPtr graph;
    const std::vector<int>* pumps;
    const std::vector<int>* valves;
    const std::vector<int>* openContainers;
    const std::vector<int>* closeContainers;
    const std::vector<std::tuple<int,int>>* tubes;
    const std::unordered_map<short int,short int> * idOpenContainerLiquidIdMap;

    std::unordered_map<std::string, std::shared_ptr<Variable>> variableMap;
    std::vector<std::shared_ptr<Rule>> rules;

    void generateDomain();
    std::shared_ptr<Predicate> generatePumpsCloseState();    

    std::shared_ptr<VariableDomain> generateDomainPumpDir(int id);
    std::shared_ptr<VariableDomain> generateDomainPumpRate(int id);
    std::shared_ptr<VariableDomain> generateDomainValve(int id);
    std::shared_ptr<VariableDomain> generateDomainCloseContainer(int id);
    std::shared_ptr<VariableDomain> generateDomainOpenContainer(int id) throw(std::invalid_argument);
    std::shared_ptr<VariableDomain> generateDomainTube(const std::tuple<int,int> & tuple);

    std::vector<int> getValvePossibleValues(int id);
    std::vector<std::shared_ptr<Variable>> generateVariablesVector(MachineGraph::EdgeVectorPtr edges);
    std::vector<VariableDomain::DomainTuple> generateDomainForValues(const std::vector<long long> & values);

    std::shared_ptr<Predicate> generateEqualityContainer(GraphPtr graph, int id) throw(std::invalid_argument);

    std::shared_ptr<ArithmeticOperable> addVariables(std::shared_ptr<Variable> var1, std::shared_ptr<Variable> var2) throw(std::invalid_argument);
    std::shared_ptr<ArithmeticOperable> addVariables(const std::vector<std::shared_ptr<Variable>> & variables) throw(std::invalid_argument);
    std::shared_ptr<ArithmeticOperable> calculateRate(std::shared_ptr<Variable> variable);
    std::shared_ptr<ArithmeticOperable> calculateId(std::shared_ptr<Variable> variable);

    std::shared_ptr<Variable> getVariable(const std::string & name);
    std::shared_ptr<IntegerNumber> getNumber(long long value);

    inline std::shared_ptr<Predicate> createEquality(const std::string & name, int value) {
        return std::make_shared<Equality>(std::make_shared<Variable>(name), getNumber(value));
    }

    inline std::shared_ptr<Predicate> createEquality(const std::string & name1, const std::string & name2) {
        return std::make_shared<Equality>(getVariable(name1), getVariable(name2));
    }

    inline std::shared_ptr<Predicate> createEquality(const std::string & name1, std::shared_ptr<ArithmeticOperable> op) {
        return std::make_shared<Equality>(getVariable(name1), op);
    }

    inline std::shared_ptr<Predicate> andPredicates(std::shared_ptr<Predicate> pred1, std::shared_ptr<Predicate> pred2) {
        return std::make_shared<Conjunction>(pred1, Conjunction::predicate_and, pred2);
    }

    inline std::shared_ptr<Predicate> orPredicates(std::shared_ptr<Predicate> pred1, std::shared_ptr<Predicate> pred2) {
        return std::make_shared<Conjunction>(pred1, Conjunction::predicate_or, pred2);
    }

    inline std::string makePumpDirName(int id) {
        return "P_" + std::to_string(id);
    }

    inline std::string makePumpRateName(int id) {
        return "R_" + std::to_string(id);
    }

    inline std::string makeValveName(int id) {
        return "V_" + std::to_string(id);
    }

    inline std::string makeContainerName(int id) {
        return "C_" + std::to_string(id);
    }

    inline std::string makeTubeName(const std::tuple<int,int> & tuple) {
        return "T_" + std::to_string(std::get<0>(tuple)) + "_" + std::to_string(std::get<1>(tuple));
    }

    inline std::string makeTubeName(int idSource, int idTarget) {
        return "T_" + std::to_string(idSource) + "_" + std::to_string(idTarget);
    }
};

#endif // GRAPHRULESGENERATOR_H
