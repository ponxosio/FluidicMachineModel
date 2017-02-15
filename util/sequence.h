#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdexcept>
#include <string>
#include <unordered_set>
#include <set>
#include <vector>

#include "fluidicmachinemodel_global.h"

class SEQUENCE_EXPORT Sequence
{
public:
    Sequence(int startValue, int endValue, const std::vector<int>* forbidenValuesVector = nullptr) throw (std::invalid_argument);
    Sequence(const std::set<int> & valuesOfSequence) throw (std::invalid_argument);
    Sequence(const std::vector<int> & valuesOfSequence) throw (std::invalid_argument);
    virtual ~Sequence();

    void reset();
    bool advanceSequence() throw(std::runtime_error);
    int getActualValue();

protected:
    int actualValue;
    int startValue;
    int endValue;
    std::unordered_set<int> forbiddenValuesSet;

    void loadSequence(const std::set<int> & valuesOfSequence) throw(std::invalid_argument);

private:
    Sequence(const Sequence & s){}
};

#endif // SEQUENCE_H
