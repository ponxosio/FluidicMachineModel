#include "machinestate.h"

MachineState::MachineState() {

}

MachineState::MachineState(const std::unordered_map<std::string, long long> & states) throw (std::invalid_argument) {
    setAllStates(states);
}

MachineState::~MachineState() {

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
    containersMap.clear();
    tubesMap.clear();
    valvesMap.clear();
    pumpsMap.clear();
    pumpsRatesMap.clear();

    for(auto varTuple: states) {
        std::string name = varTuple.first;
        long long value = varTuple.second;

        const char firstChar = name.at(0);
        switch (firstChar) {
        case 'C':
            containersMap.insert(std::make_pair(name,value));
            break;
        case 'T':
            tubesMap.insert(std::make_pair(name,value));
            break;
        case 'P':
            pumpsMap.insert(std::make_pair(name,value));
            break;
        case 'R':
            pumpsRatesMap.insert(std::make_pair(name,value));
            break;
        case 'V':
            valvesMap.insert(std::make_pair(name,value));
            break;
        default:
            throw (std::invalid_argument("unknow var format, " + name));
        }
    }
}

int MachineState::getContainerId(const std::string & containerNameVar) throw(std::invalid_argument) {
    if (containerNameVar.at(0) == 'C') {
        std::string id = containerNameVar.substr(2);
        try {
            int idValue = std::stoi(id, nullptr);
            return idValue;
        } catch (std::invalid_argument & e) {
            throw (std::invalid_argument("error extracting id from the variable " + containerNameVar + ", error message: " + e.what()));
        }
    } else {
        throw(std::invalid_argument("variable " + containerNameVar + " is not a container variable"));
    }
}

long long MachineState::getContainerState(int id) throw(std::invalid_argument) {
    std::string varName = "C_" + std::to_string(id);
    auto it = containersMap.find(varName);

    if (it != containersMap.end()) {
        long long cState = it->second;
        return cState;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a container"));
    }
}

void MachineState::setContainerState(int id, long long state) throw(std::invalid_argument) {
    std::string varName = "C_" + std::to_string(id);
    auto it = containersMap.find(varName);

    if (it != containersMap.end()) {
        containersMap.insert(std::make_pair(varName, state));
    } else {
        it->second = state;
    }
}

const std::unordered_map<std::string, long long> & MachineState::getAllContainersVar() {
    return containersMap;
}

std::tuple<int,int> MachineState::getTubeId(const std::string & tubeNameVar) throw(std::invalid_argument) {
    if (tubeNameVar.at(0) == 'T') {
        int lastUnderScore = tubeNameVar.find_last_of("_");
        std::string idSource = tubeNameVar.substr(2,lastUnderScore);
        std::string idTarget = tubeNameVar.substr(lastUnderScore + 1);
        try {
            int sourceValue = std::stoi(idSource, nullptr);
            int targetValue = std::stoi(idTarget, nullptr);
            return std::make_tuple(sourceValue, targetValue);
        } catch (std::invalid_argument & e) {
            throw (std::invalid_argument("error extracting id from the variable " + tubeNameVar + ". Error message: " + e.what()));
        }
    } else {
        throw(std::invalid_argument("variable " + tubeNameVar + " is not a tube variable"));
    }
}

long long MachineState::getTubeState(int idSource, int idTarget) throw(std::invalid_argument) {
    std::string varName = "T_" + std::to_string(idSource) + "_" + std::to_string(idTarget);
    auto it = tubesMap.find(varName);

    if (it != tubesMap.end()) {
        long long cState = it->second;
        return cState;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a tube"));
    }
}

void MachineState::setTubeState(int idSource, int idTarget, long long state) {
    std::string varName = "T_" + std::to_string(idSource) + "_" + std::to_string(idTarget);
    auto it = tubesMap.find(varName);

    if (it != tubesMap.end()) {
        tubesMap.insert(varName, state);
    } else {
        it->second = state;
    }
}

const std::unordered_map<std::string, long long> & MachineState::getAllTubes() {
    return tubesMap;
}

int MachineState::getPumpId(const std::string & pumpVarName) throw(std::invalid_argument) {
    if (pumpVarName.at(0) == 'P') {
        std::string id = pumpVarName.substr(2);
        try {
            int idValue = std::stoi(id, nullptr);
            return idValue;
        } catch (std::invalid_argument & e) {
            throw (std::invalid_argument("error extracting id from the variable " + pumpVarName + ", error message: " + e.what()));
        }
    } else {
        throw(std::invalid_argument("variable " + pumpVarName + " is not a pump variable"));
    }
}

long long MachineState::getPumpDir(int id) throw (std::invalid_argument) {
    std::string varName = "P_" + std::to_string(id);
    auto it = pumpsMap.find(varName);

    if (it != pumpsMap.end()) {
        long long cState = it->second;
        return cState;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a pump"));
    }
}

long long MachineState::getPumpRate(int id) throw (std::invalid_argument) {
    std::string varName = "R_" + std::to_string(id);
    auto it = pumpsRatesMap.find(varName);

    if (it != pumpsRatesMap.end()) {
        long long cState = it->second;
        return cState;
    } else {
        throw(std::invalid_argument(std::to_string(id) + " is not present or is not a pump's rate"));
    }
}

const std::unordered_map<std::string, long long> & MachineState::getAllPumpsDirVar() {
    return pumpsMap;
}

const std::unordered_map<std::string, long long> & MachineState::getAllPumpsRateVar() {
    return pumpsRatesMap;
}

int MachineState::getValveId(const std::string & valveVarname) throw(std::invalid_argument) {
    if (valveVarname.at(0) == 'V') {
        std::string id = valveVarname.substr(2);
        try {
            int idValue = std::stoi(id, nullptr);
            return idValue;
        } catch (std::invalid_argument & e) {
            throw (std::invalid_argument("error extracting id from the variable " + valveVarname + ", error message: " + e.what()));
        }
    } else {
        throw(std::invalid_argument("variable " + valveVarname + " is not a valve variable"));
    }
}

long long MachineState::getValvePosition(int id) {
    std::string varName = "V_" + std::to_string(id);
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
