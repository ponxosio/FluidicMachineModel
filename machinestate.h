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
    MachineState(const std::vector<std::tuple<std::string, int>> & states);
    virtual ~MachineState();

    std::vector<std::tuple<std::string, int>> getAllValues();
    std::vector<std::tuple<std::string, int>> getALLContainersTubes();
    void setAllStates(const std::vector<std::tuple<std::string, int>> & states);

    int getContainer(int id) throw(std::invalid_argument);
    void setContainer(int id, int state);
    const std::vector<std::tuple<std::string, int>> & getAllContainersVar();
    std::vector<std::tuple<int, int>> getAllContainers();

    int getValve(int id) throw(std::invalid_argument);
    void setValve(int id, int state);
    const std::vector<std::tuple<std::string, int>> & getAllValvesVar();
    std::vector<std::tuple<int, int>> getAllValves();

    int getTube(int idSource, int idTarget) throw(std::invalid_argument);
    void setContainer(int idSource, int idTarget, int state);
    const std::vector<std::tuple<std::string, int>> & getAllTubesVar();
    std::vector<std::tuple<int, int>> getAllTubes();

    int getPump(int id) throw(std::invalid_argument);
    void setPump(int id, int state);
    const std::vector<std::tuple<std::string, int>> & getAllPumpsVar();
    std::vector<std::tuple<int, std::tuple<int, double>>> getAllPumps();

    double getPumpRate(int id) throw(std::invalid_argument);
    void setPumpRate(int id, double rate);
    const std::vector<std::tuple<std::string, double>> & getAllPumpRatesVar();

    MachineState getDifferences(const MachineState & otherState);

protected:
    std::vector<std::tuple<std::string, int>> containers;
    std::vector<std::tuple<std::string, int>> valves;
    std::vector<std::tuple<std::string, int>> tubes;
    std::vector<std::tuple<std::string, int>> pumps;
    std::unordered_map<std::string, double> pumpsRate;
};

#endif // MACHINESTATE_H
