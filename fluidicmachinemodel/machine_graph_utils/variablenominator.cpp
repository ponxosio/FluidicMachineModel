#include "variablenominator.h"

int VariableNominator::getValveId(const std::string & valveVarname) throw(std::invalid_argument) {
    if (valveVarname.at(0) == VALVE_PREFIX) {
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

int VariableNominator::getPumpId(const std::string & pumpVarName) throw(std::invalid_argument) {
    if (pumpVarName.at(0) == PUMP_PREFIX ||
        pumpVarName.at(0) == RATE_PREFIX)
    {
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

std::tuple<int,int> VariableNominator::getTubeId(const std::string & tubeNameVar) throw(std::invalid_argument) {
    if (tubeNameVar.at(0) == TUBE_PREFIX) {
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

int VariableNominator::getContainerId(const std::string & containerNameVar) throw(std::invalid_argument) {
    if (containerNameVar.at(0) == CONTAINER_PREFIX) {
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

std::string VariableNominator::getValveVarName(int id) {
    return std::string(1, VALVE_PREFIX) + "_" + std::to_string(id);
}

std::string VariableNominator::getPumpVarName(int id) {
    return std::string(1, PUMP_PREFIX) + "_" + std::to_string(id);
}

std::string VariableNominator::getContainerVarName(int id) {
    return std::string(1, CONTAINER_PREFIX) + "_" + std::to_string(id);
}

std::string VariableNominator::getTubeVarName(int idSource, int idTarget) {
    return std::string(1, TUBE_PREFIX) + "_" + std::to_string(idSource) + "_" +  std::to_string(idTarget);
}

std::string VariableNominator::getPumpRateVarName(int id) {
    return std::string(1, RATE_PREFIX) + "_" + std::to_string(id);
}

VariableNominator::VariableType VariableNominator::getVariableType(const std::string & varName) throw (std::invalid_argument){
    VariableType type;

    char init = varName.at(0);
    switch (init) {
    case TUBE_PREFIX:
        type = tube;
        break;
    case CONTAINER_PREFIX:
        type = container;
        break;
    case VALVE_PREFIX:
        type = valve;
        break;
    case PUMP_PREFIX:
        type = pump;
        break;
    case RATE_PREFIX:
        type = rate;
        break;
    default:
        throw(std::invalid_argument("unknow prefix: " + std::string(1, init)));
        break;
    }
    return type;
}
