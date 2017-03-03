#ifndef MACHINESTATE_H
#define MACHINESTATE_H

#define LONG_LONG_DIGITS_SIZE 18

#include <bitset>
#include <cmath>
#include <sstream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "machinegraph.h"
#include "machine_graph_utils/variablenominator.h"

class MachineState
{
public: 
    MachineState(std::shared_ptr<MachineGraph> graph,
                 short int ratePrecisionInteger,
                 short int ratePrecisionDecimal) throw(std::invalid_argument);
    virtual ~MachineState();

    short int getMaxOpenContainers();

    std::unordered_map<std::string, long long> getAllValues();
    std::unordered_map<std::string, long long> getAllContainersTubes();
    void setAllStates(const std::unordered_map<std::string, long long> & states) throw (std::invalid_argument);

    long long getContainerState(int id) throw(std::invalid_argument);
    void overrideContainerState(int id, long long state) throw(std::invalid_argument);
    void addContainerState(int id, long long state) throw(std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllContainersVar();

    long long getTubeState(int idSource, int idTarget) throw(std::invalid_argument);
    void overrideTubeState(int idSource, int idTarget, long long state) throw(std::invalid_argument);
    void addTubeState(int idSource, int idTarget, long long state) throw(std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllTubes();

    long long getPumpDir(int id) throw (std::invalid_argument);
    float getPumpRate(int id) throw (std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllPumpsDirVar();
    const std::unordered_map<std::string, long long> & getAllPumpsRateVar();

    long long getValvePosition(int id) throw(std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllValves();

    long long rateToInt(float rate) throw(std::overflow_error);
    float rateToFloat(long long rate);
    long long generateState(int liquidId, float rate) throw(std::overflow_error);

    std::string toString();

    inline short int getRatePrecisionInteger() {
        return ratePrecisionInteger;
    }

    inline short int getRatePrecisionDecimal() {
        return ratePrecisionDecimal;
    }

protected:
    std::shared_ptr<MachineGraph> graph;
    short int ratePrecisionInteger;
    short int ratePrecisionDecimal;

    std::unordered_map<std::string, long long> containersMap;
    std::unordered_map<std::string, long long> tubesMap;
    std::unordered_map<std::string, long long> valvesMap;
    std::unordered_map<std::string, long long> pumpsMap;
    std::unordered_map<std::string, long long> pumpsRatesMap;

    void overrideValvePosition(const std::string & name, long long state) throw(std::invalid_argument);
    void overridePumpDir(const std::string & name, long long state) throw(std::invalid_argument);
    void overridePumpRate(const std::string & name, long long state) throw(std::invalid_argument);
    void overrideContainerState(const std::string & name, long long state) throw(std::invalid_argument);
    void overrideTubeState(const std::string & name, long long state) throw(std::invalid_argument);

    void analizeGraph() throw(std::invalid_argument);
    void insertPump(int id) throw(std::invalid_argument);
    void insertPumpRate(int id) throw(std::invalid_argument);
    void insertValve(int id) throw(std::invalid_argument);
    void insertContainer(int id) throw(std::invalid_argument);
    void insertTube(int idSource, int idTarget) throw(std::invalid_argument);

    long long addStates(long long state1, long long state2) throw(std::invalid_argument);

    inline long long getRate(long long state) {
        return abs(state % (long long) pow(10,ratePrecisionDecimal + ratePrecisionInteger));
    }

    inline long long getId(long long state) {
        return abs(trunc(state / pow(10, ratePrecisionDecimal + ratePrecisionInteger)));
    }

    std::string mapToString(const std::unordered_map<std::string, long long> & map);
};

#endif // MACHINESTATE_H
