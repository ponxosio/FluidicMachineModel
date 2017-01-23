#include "sequence.h"

Sequence::Sequence(int startValue, int endValue, const std::vector<int>* forbidenValuesVector)
    throw (std::invalid_argument)
{
    if (startValue <= endValue) {
        this->startValue = startValue;
        this->actualValue = startValue;
        this->endValue = endValue;

        if (forbidenValuesVector) {
            for(int value: *forbidenValuesVector) {
                forbiddenValuesSet.insert(value);
            }
        }
    } else {
        throw(std::invalid_argument("start value: " + std::to_string(startValue) + " must be bigger than end value: " + std::to_string(endValue)));
    }
}

Sequence::Sequence(const std::set<int> & valuesOfSequence) throw(std::invalid_argument) {
    try {
        loadSequence(valuesOfSequence);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("Erro while loading sequence: " + std::string(e.what())));
    }
}

Sequence::Sequence(const std::vector<int> & valuesOfSequence) throw (std::invalid_argument) {
    std::set<int> tempSet;
    for(int value: valuesOfSequence) {
        tempSet.insert(value);
    }

    try{
        loadSequence(tempSet);
    } catch (std::invalid_argument & e) {
        throw(std::invalid_argument("Erro while loading sequence: " + std::string(e.what())));
    }
}

Sequence::~Sequence() {

}

int Sequence::getActualValue() {
    return actualValue;
}

void Sequence::reset() {
    actualValue = startValue;
}

bool Sequence::hasEnded() {
    return (actualValue > endValue);
}

int Sequence::nextValue() throw(std::runtime_error) {
    if (!hasEnded()) {
        auto it = forbiddenValuesSet.find(actualValue);
        while(it != forbiddenValuesSet.end()) {
            actualValue++;
            it = forbiddenValuesSet.find(actualValue);
        }
        return actualValue++;
    } else {
        throw(std::runtime_error("trying to get next value from a finished sequence"));
    }
}

void Sequence::loadSequence(const std::set<int> & valuesOfSequence) throw(std::invalid_argument) {
    auto it = valuesOfSequence.begin();
    if (it != valuesOfSequence.end()) {
        startValue = *it;
        actualValue = startValue;
        it++;

        int previousValue = startValue;
        for (; it != valuesOfSequence.end(); ++it) {
            while (*it > (previousValue + 1)) {
                forbiddenValuesSet.insert(previousValue);
                previousValue++;
            }
        }
        endValue = previousValue;
    } else {
        throw(std::invalid_argument("imposible to create sequence, empty map received"));
    }
}
