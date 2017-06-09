#include "machinestate.h"

MachineState::MachineState(short int ratePrecisionInteger, short int ratePrecisionDecimal) {
    this->ratePrecisionInteger = ratePrecisionInteger;
    this->ratePrecisionDecimal = ratePrecisionDecimal;
}

MachineState::MachineState(std::shared_ptr<MachineGraph> graph,
                           short int ratePrecisionInteger,
                           short int ratePrecisionDecimal)
    throw(std::invalid_argument)
{
    this->ratePrecisionInteger = ratePrecisionInteger;
    this->ratePrecisionDecimal = ratePrecisionDecimal;
    analizeGraph(graph);
}

MachineState::~MachineState() {

}

short int MachineState::getMaxOpenContainers() {
    short int digits = LONG_LONG_DIGITS_SIZE - (ratePrecisionDecimal + ratePrecisionInteger);
    double maxNumber = pow(10, digits) - 1;
    double n = std::log2(maxNumber);
    return (short int) std::round(n);
}

std::unordered_map<std::string, long long> MachineState::getAllValues() {
    std::unordered_map<std::string, long long> allMap;
    allMap.insert(containersMap.begin(), containersMap.end());
    allMap.insert(tubesMap.begin(), tubesMap.end());
    allMap.insert(valvesMap.begin(), valvesMap.end());
    allMap.insert(pumpsMap.begin(), pumpsMap.end());
    allMap.insert(pumpsRatesMap.begin(), pumpsRatesMap.end());
    return allMap;
}

std::unordered_map<std::string, long long> MachineState::getAllContainersTubes() {
    std::unordered_map<std::string,long long> containerTubeMap;
    containerTubeMap.insert(containersMap.begin(), containersMap.end());
    containerTubeMap.insert(tubesMap.begin(), tubesMap.end());
    return containerTubeMap;
}

void MachineState::setAllStates(const std::unordered_map<std::string, long long> & states) throw (std::invalid_argument) {
    setMapToZero(containersMap);
    setMapToZero(tubesMap);
    setMapToZero(pumpsMap);
    setMapToZero(pumpsRatesMap);
    setMapToZero(valvesMap);

    for(auto varTuple: states) {
        std::string name = varTuple.first;
        long long value = varTuple.second;
        try {
            VariableNominator::VariableType nameType = VariableNominator::getVariableType(name);
            switch (nameType) {
            case VariableNominator::container:
                overrideContainerState(name, value);
                break;
            case VariableNominator::tube:
                overrideTubeState(name, value);
                break;
            case VariableNominator::pump:
                overridePumpDir(name, value);
                break;
            case VariableNominator::rate:

                overridePumpRate(name, value);
                break;
            case VariableNominator::valve:
                overrideValvePosition(name, value);
                break;
            default:
                break;
            }
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("error setting variable " + name + ". Error:" + std::string(e.what())));
        }
    }
}

void MachineState::emplaceContainerVar(int id) {
    std::string varName = VariableNominator::getContainerVarName(id);
    auto found = containersMap.find(varName);
    if (found == containersMap.end()) {
        containersMap.insert(std::make_pair(varName, 0));
    }
}

long long MachineState::getContainerState(int id) throw(std::invalid_argument) {   
    std::string varName = VariableNominator::getContainerVarName(id);
    auto it = containersMap.find(varName);

    if (it != containersMap.end()) {
        long long cState = it->second;
        return cState;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a container"));
    }
}

void MachineState::overrideContainerState(int id, long long state) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getContainerVarName(id);
    auto it = containersMap.find(varName);

    if (it != containersMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a container"));
    }
}

void MachineState::addContainerState(int id, long long state) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getContainerVarName(id);
    auto it = containersMap.find(varName);

    if (it != containersMap.end()) {
        try {
            it->second = addStates(state, it->second);
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("imposible to add container " + std::to_string(id) +
                                        " state. Error message: " + std::string(e.what())));
        }
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a container"));
    }
}

const std::unordered_map<std::string, long long> & MachineState::getAllContainersVar() {
    return containersMap;
}

void MachineState::emplaceTubeVar(int idSource, int idTarget) {
    std::string varName = VariableNominator::getTubeVarName(idSource, idTarget);
    auto found = tubesMap.find(varName);
    if (found == tubesMap.end()) {
        tubesMap.insert(std::make_pair(varName, 0));
    }
}

long long MachineState::getTubeState(int idSource, int idTarget) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getTubeVarName(idSource, idTarget);
    auto it = tubesMap.find(varName);

    if (it != tubesMap.end()) {
        long long cState = it->second;
        return cState;
    } else {
        throw(std::invalid_argument(std::to_string(idSource) + "->" + std::to_string(idTarget)  + " is not present or is not a tube"));
    }
}

void MachineState::addTubeState(int idSource, int idTarget, long long state) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getTubeVarName(idSource, idTarget);
    auto it = tubesMap.find(varName);

    if (it != tubesMap.end()) {
        try {
            it->second = addStates(state, it->second);
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument(std::to_string(idSource) + "->" + std::to_string(idTarget)  +
                                        " imposible to add state. Error message: " + std::string(e.what())));
        }
    } else {
        throw(std::invalid_argument(std::to_string(idSource) + "->" + std::to_string(idTarget)  + " is not present or is not a tube"));
    }
}

void MachineState::overrideTubeState(int idSource, int idTarget, long long state) throw (std::invalid_argument) {
    std::string varName = VariableNominator::getTubeVarName(idSource, idTarget);
    auto it = tubesMap.find(varName);

    if (it != tubesMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument(std::to_string(idSource) + "->" + std::to_string(idTarget)  + " is not present or is not a tube"));
    }
}

const std::unordered_map<std::string, long long> & MachineState::getAllTubes() {
    return tubesMap;
}

void MachineState::emplacePumpVar(int id) {
    std::string varDirName = VariableNominator::getPumpVarName(id);
    std::string varRateName = VariableNominator::getPumpRateVarName(id);

    auto found = pumpsMap.find(varDirName);
    if (found == pumpsMap.end()) {
        pumpsMap.insert(std::make_pair(varDirName, 0));
    }

    auto foundR = pumpsRatesMap.find(varRateName);
    if (foundR == pumpsRatesMap.end()) {
        pumpsRatesMap.insert(std::make_pair(varRateName, 0));
    }
}

long long MachineState::getPumpDir(int id) throw (std::invalid_argument) {
    std::string varName = VariableNominator::getPumpVarName(id);
    auto it = pumpsMap.find(varName);

    if (it != pumpsMap.end()) {
        long long cState = it->second;
        return cState;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a pump"));
    }
}

float MachineState::getPumpRate(int id) throw (std::invalid_argument) {
    std::string varName = VariableNominator::getPumpRateVarName(id);
    auto it = pumpsRatesMap.find(varName);

    if (it != pumpsRatesMap.end()) {
        long long cState = it->second;
        return rateToDouble(cState);
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a pump's rate"));
    }
}

void MachineState::overridePumpRateState(int id, long long state) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getPumpRateVarName(id);
    auto it = pumpsRatesMap.find(varName);

    if (it != pumpsRatesMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a pump's rate"));
    }
}

void MachineState::overridePumpDirState(int id, long long state) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getPumpVarName(id);
    auto it = pumpsMap.find(varName);

    if (it != pumpsMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a pump's dir"));
    }
}

const std::unordered_map<std::string, long long> & MachineState::getAllPumpsDirVar() {
    return pumpsMap;
}

const std::unordered_map<std::string, long long> & MachineState::getAllPumpsRateVar() {
    return pumpsRatesMap;
}

void MachineState::emplaceValveVar(int id) {
    std::string varName = VariableNominator::getValveVarName(id);
    auto found = valvesMap.find(varName);
    if (found == valvesMap.end()) {
        valvesMap.insert(std::make_pair(varName, 0));
    }
}

long long MachineState::getValvePosition(int id)
    throw(std::invalid_argument)
{
    std::string varName = VariableNominator::getValveVarName(id);
    auto it = valvesMap.find(varName);

    if (it != valvesMap.end()) {
        long long cState = it->second;
        return cState;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a valve"));
    }
}

const std::unordered_map<std::string, long long> & MachineState::getAllValves() {
    return valvesMap;
}

long long MachineState::addStates(long long state1, long long state2) throw(std::invalid_argument) {
    float maxRate = pow(10, ratePrecisionDecimal + ratePrecisionInteger) - 1.0;

    if (state1 == 0 && state2 == 0) {
        return 0;
    } else if (state1 == 0 && state2 != 0) {
        return state2;
    } else if (state1 != 0 && state2 == 0) {
        return state1;
    } else {
        long long rate1 = getRate(state1);
        long long rate2 = getRate(state2);

        if (maxRate > (rate1 + rate2)) {
            try {
                long long id1 = getId(state1);
                long long id2 = getId(state2);
                return (id1 + id2) * std::pow(10, ratePrecisionInteger + ratePrecisionDecimal) + (rate1 + rate2);
            } catch (std::overflow_error & e) {
                throw(std::invalid_argument("imposible to add " + std::to_string(state1) + " and " + std::to_string(state2) +
                                            ", overflow error. Message: " + std::string(e.what())));
            }
        } else {
            throw(std::invalid_argument("the states surpass the maxium rate permited"));
        }
    }
}

long long MachineState::rateToInt(float rate) throw (std::overflow_error) {
    float maxRate = pow(10, ratePrecisionDecimal + ratePrecisionInteger) - 1.0;
    if (rate < maxRate) {
        long integer = truncf(rate);
        long decimal = roundf((rate - integer) * pow(10, ratePrecisionDecimal));
        return (integer*pow(10, ratePrecisionDecimal) + decimal);
    } else {
        throw(std::overflow_error(std::to_string(rate) + " is too big, max value " + std::to_string(maxRate)));
    }
}

double MachineState::rateToDouble(long long rate) {
    float integer = trunc(rate/pow(10, ratePrecisionDecimal));
    float decimal = rate % (long long)pow(10, ratePrecisionDecimal);
    return integer + (decimal/pow(10, ratePrecisionDecimal));
}

long long MachineState::generateState(int liquidId, float rate) throw(std::overflow_error) {
    long long maxId = pow(10,LONG_LONG_DIGITS_SIZE - (ratePrecisionDecimal + ratePrecisionInteger)) - 1.0;
    try {
        long long intRate = rateToInt(rate);
        if (liquidId < maxId) {
            return (liquidId * pow(10, ratePrecisionDecimal + ratePrecisionInteger) + intRate);
        } else {
            throw(std::overflow_error( std::to_string(liquidId) + " is too big, max id " + std::to_string(maxId)));
        }
    } catch (std::overflow_error & e) {
        throw(std::overflow_error("generateState: " + std::string(e.what())));
    }
}

void MachineState::analizeGraph(std::shared_ptr<MachineGraph> graph) throw (std::invalid_argument) {
    for (int pumpId : graph->getPumpsIdsSet()) {
        try {
            insertPump(pumpId);
            insertPumpRate(pumpId);
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("Error while analyzing pumps: " + std::string(e.what())));
        }
    }

    for (int valveId : graph->getValvesIdsSet()) {
        try {
            insertValve(valveId);
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("Error while analyzing valves: " + std::string(e.what())));
        }
    }

    for (int opencontainerId : graph->getOpenContainersIdsSet()) {
        try {
            insertContainer(opencontainerId);
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("Error while analyzing open containers: " + std::string(e.what())));
        }
    }

    for (int closeContainerId : graph->getCloseContainersIdsSet()) {
        try {
            insertContainer(closeContainerId);
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("Error while analyzing closeContainerId: " + std::string(e.what())));
        }
    }

    for (auto edgeTuple : *graph->getAlledgesMap().get()) {
        try {
            MachineGraph::GraphType::EdgeTypePtr edgePtr = edgeTuple.second;
            insertTube(edgePtr->getIdSource(), edgePtr->getIdTarget());
        } catch (std::invalid_argument & e) {
            throw(std::invalid_argument("Error while analyzing tubes: " + std::string(e.what())));
        }
    }
}

void MachineState::insertPump(int id) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getPumpVarName(id);

    auto it = pumpsMap.find(varName);
    if (it == pumpsMap.end()) {
        pumpsMap.insert(std::make_pair(varName, 0));
    } else {
        throw(std::invalid_argument("cannot insert pump, already exists key: " + varName));
    }
}

void MachineState::insertPumpRate(int id) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getPumpRateVarName(id);

    auto it = pumpsRatesMap.find(varName);
    if (it == pumpsRatesMap.end()) {
        pumpsRatesMap.insert(std::make_pair(varName, 0));
    } else {
        throw(std::invalid_argument("cannot insert pump rate, already exists key: " + varName));
    }
}

void MachineState::insertValve(int id) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getValveVarName(id);

    auto it = valvesMap.find(varName);
    if (it == valvesMap.end()) {
        valvesMap.insert(std::make_pair(varName, 0));
    } else {
        throw(std::invalid_argument("cannot insert valve, already exists key: " + varName));
    }
}

void MachineState::insertContainer(int id) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getContainerVarName(id);

    auto it = containersMap.find(varName);
    if (it == containersMap.end()) {
        containersMap.insert(std::make_pair(varName, 0));
    } else {
        throw(std::invalid_argument("cannot insert container, already exists key: " + varName));
    }
}

void MachineState::insertTube(int idSource, int idTarget) throw(std::invalid_argument) {
    std::string varName = VariableNominator::getTubeVarName(idSource, idTarget);

    auto it = tubesMap.find(varName);
    if (it == tubesMap.end()) {
        tubesMap.insert(std::make_pair(varName, 0));
    } else {
        throw(std::invalid_argument("cannot insert tube, already exists key: " + varName));
    }
}

void MachineState::setMapToZero(const std::unordered_map<std::string, long long> & map) {
    for(auto actualPair: map) {
        actualPair.second = 0;
    }
}

void MachineState::overrideValvePosition(const std::string & name, long long state) throw(std::invalid_argument) {
    auto it = valvesMap.find(name);
    if (it != valvesMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument("imposible to override valve state: " + name + ", is not present."));
    }
}

void MachineState::overridePumpDir(const std::string & name, long long state) throw(std::invalid_argument) {
    auto it = pumpsMap.find(name);
    if (it != pumpsMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument("imposible to override pump's dir state: " + name + ", is not present."));
    }
}

void MachineState::overridePumpRate(const std::string & name, long long state) throw(std::invalid_argument) {
    auto it = pumpsRatesMap.find(name);
    if (it != pumpsRatesMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument("imposible to override pump's rate state: " + name + ", is not present."));
    }
}

void MachineState::overrideContainerState(const std::string & name, long long state) throw(std::invalid_argument) {
    auto it = containersMap.find(name);
    if (it != containersMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument("imposible to override container state: " + name + ", is not present."));
    }
}

void MachineState::overrideTubeState(const std::string & name, long long state) throw(std::invalid_argument) {
    auto it = tubesMap.find(name);
    if (it != tubesMap.end()) {
        it->second = state;
    } else {
        throw(std::invalid_argument("imposible to override tube's state: " + name + ", is not present."));
    }
}























