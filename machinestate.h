#ifndef MACHINESTATE_H
#define MACHINESTATE_H

#define CONTAINER_PREFIX "C"
#define TUBE_PREFIX "T"
#define VALVE_PREFIX "V"
#define PUMP_PREFIX "P"

#include <bitset>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>

class MachineState
{
public:
    MachineState();
    MachineState(const std::unordered_map<std::string, long long> & states) throw (std::invalid_argument);
    virtual ~MachineState();

    std::unordered_map<std::string, long long> getAllValues();
    std::unordered_map<std::string, long long> getAllContainersTubes();
    void setAllStates(const std::vector<std::tuple<std::string, long long>> & states) throw (std::invalid_argument);

    int getContainerId(const std::string & containerNameVar) throw(std::invalid_argument);
    long long getContainerState(int id) throw(std::invalid_argument);
    void setContainerState(int id, long long state) throw(std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllContainersVar();

    std::tuple<int,int> getTubeId(const std::string & tubeNameVar) throw(std::invalid_argument);
    long long getTubeState(int idSource, int idTarget) throw(std::invalid_argument);
    void setTubeState(int idSource, int idTarget, long long state);
    const std::unordered_map<std::string, long long> & getAllTubes();

    int getPumpId(const std::string & pumpVarName) throw(std::invalid_argument);
    long long getPumpDir(int id) throw (std::invalid_argument);
    long long getPumpRate(int id) throw (std::invalid_argument);
    const std::unordered_map<std::string, long long> & getAllPumpsDirVar();
    const std::unordered_map<std::string, long long> & getAllPumpsRateVar();

    int getValveId(const std::string & valveVarname) throw(std::invalid_argument);
    long long getValvePosition(int id);
    const std::unordered_map<std::string, long long> & getAllValves();

protected:
    std::unordered_map<std::string, long long> containersMap;
    std::unordered_map<std::string, long long> tubesMap;
    std::unordered_map<std::string, long long> valvesMap;
    std::unordered_map<std::string, long long> pumpsMap;
    std::unordered_map<std::string, long long> pumpsRatesMap;
};

#endif // MACHINESTATE_H
