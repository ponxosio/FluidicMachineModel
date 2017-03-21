#ifndef VARIABLENOMINATOR_H
#define VARIABLENOMINATOR_H

#define CONTAINER_PREFIX 'C'
#define TUBE_PREFIX 'T'
#define VALVE_PREFIX 'V'
#define PUMP_PREFIX 'P'
#define RATE_PREFIX 'R'

#include <stdexcept>
#include <string>
#include <tuple>

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class VARIABLE_NOMINATOR_EXPORT VariableNominator
{
public:
    typedef enum VariableType_ {
        valve,
        rate,
        pump,
        container,
        tube
    } VariableType;

    static int getValveId(const std::string & valveVarname) throw(std::invalid_argument);
    static int getPumpId(const std::string & pumpVarName) throw(std::invalid_argument);
    static std::tuple<int,int> getTubeId(const std::string & tubeNameVar) throw(std::invalid_argument);
    static int getContainerId(const std::string & containerNameVar) throw(std::invalid_argument);

    static std::string getValveVarName(int id);
    static std::string getPumpVarName(int id);
    static std::string getPumpRateVarName(int id);
    static std::string getContainerVarName(int id);
    static std::string getTubeVarName(int idSource, int idTarget);

    static VariableType getVariableType(const std::string & varName) throw (std::invalid_argument);
};

#endif // VARIABLENOMINATOR_H
