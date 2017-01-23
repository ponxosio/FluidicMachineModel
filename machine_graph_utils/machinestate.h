#ifndef MACHINESTATE_H
#define MACHINESTATE_H

#define LONG_LONG_DIGITS_SIZE 18

#define CONTAINER_PREFIX "C"
#define TUBE_PREFIX "T"
#define VALVE_PREFIX "V"
#define PUMP_PREFIX "P"

#include <bitset>
#include <cmath>
#include <string>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

class MachineState
{
public: 
    MachineState(short int ratePrecisionInteger, short int ratePrecisionDecimal);
    MachineState(short int ratePrecisionInteger,
                 short int ratePrecisionDecimal,
                 const std::unordered_map<std::string, long long> & states) throw (std::invalid_argument);
    virtual ~MachineState();

    short int getMaxOpenContainers();

    std::unordered_map<std::string, long long> getAllValues();
    std::unordered_map<std::string, long long> getAllContainersTubes();
    void setAllStates(const std::unordered_map<std::string, long long> & states) throw (std::invalid_argument);

    int getContainerId(const std::string & containerNameVar) throw(std::invalid_argument);
    long long getContainerState(int id) throw(std::invalid_argument);
    void overrideContainerState(int id, long long state) throw(std::invalid_argument);
    void addContainerState(int id, long long state) throw(std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllContainersVar();

    std::tuple<int,int> getTubeId(const std::string & tubeNameVar) throw(std::invalid_argument);
    long long getTubeState(int idSource, int idTarget) throw(std::invalid_argument);
    void overrideTubeState(int idSource, int idTarget, long long state);
    void addTubeState(int idSource, int idTarget, long long state) throw(std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllTubes();

    int getPumpId(const std::string & pumpVarName) throw(std::invalid_argument);
    long long getPumpDir(int id) throw (std::invalid_argument);
    float getPumpRate(int id) throw (std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllPumpsDirVar();
    const std::unordered_map<std::string, long long> & getAllPumpsRateVar();

    int getValveId(const std::string & valveVarname) throw(std::invalid_argument);
    long long getValvePosition(int id);
    const std::unordered_map<std::string, long long> & getAllValves();

    long long rateToInt(float rate) throw(std::overflow_error);
    float rateToFloat(long long rate);
    long long generateState(int liquidId, float rate) throw(std::runtime_error);

    inline short int getRatePrecisionInteger() {
        return ratePrecisionInteger;
    }

    inline short int getRatePrecisionDecimal() {
        return ratePrecisionDecimal;
    }

protected:
    short int ratePrecisionInteger;
    short int ratePrecisionDecimal;

    std::unordered_map<std::string, long long> containersMap;
    std::unordered_map<std::string, long long> tubesMap;
    std::unordered_map<std::string, long long> valvesMap;
    std::unordered_map<std::string, long long> pumpsMap;
    std::unordered_map<std::string, long long> pumpsRatesMap;

    long long addStates(long long state1, long long state2) throw(std::overflow_error);

    inline long long getRate(long long state) {
        return abs(state % (long long) pow(10,ratePrecisionDecimal + ratePrecisionInteger));
    }

    inline long long getId(long long state) {
        return abs(trunc(state / pow(10, ratePrecisionDecimal + ratePrecisionInteger)));
    }
};

#endif // MACHINESTATE_H
