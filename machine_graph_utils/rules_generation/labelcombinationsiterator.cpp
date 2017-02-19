#include "labelcombinationsiterator.h"

LabelCombinationsIterator::LabelCombinationsIterator(const LabelTypeTubeMap & labelMap, const std::vector<Label::LabelType> & types) {
    maxSequenceIndex = 0;
    statesSequenceVector.reserve(labelMap.size());

    int i = 0;
    for(const auto & tuple : labelMap) {
        Label label = tuple.second;

        std::set<int> valuesOfSequence = {0};
        for(Label::LabelType type: types) {
            if (label.hasLabelMask(type)) {
                valuesOfSequence.insert(type);
            }
        }

        statesSequenceVector.insert(statesSequenceVector.begin() + i, std::make_tuple(tuple.first, new Sequence(valuesOfSequence)));
        i++;
    }
}

LabelCombinationsIterator::~LabelCombinationsIterator()
{
    //remove the Sequence variables
    for(auto tuple: statesSequenceVector) {
        Sequence* seq = std::get<1>(tuple);
        delete seq;
    }
}

bool LabelCombinationsIterator::calculateNext() {
    if (maxSequenceIndex < statesSequenceVector.size()) {
        fillActualMap();
        advanceSequence();
        return true;
    } else {
        return false;
    }
}

void LabelCombinationsIterator::fillActualMap() {
    actualTubesLabels.clear();
    actualLabelSet.clear();

    for(const auto & tuple: statesSequenceVector) {
        const std::tuple<int,int> tubeId = std::get<0>(tuple);
        Sequence* actualSeq = std::get<1>(tuple);
        int actualValue = actualSeq->getActualValue();

        Label label;
        if (actualValue != 0) {
            label.setLabel((Label::LabelType) actualValue);
        }

        actualLabelSet.insert(label);
        actualTubesLabels.insert(std::make_pair(tubeId, label));
    }
}

void LabelCombinationsIterator::advanceSequence() {
    int localIndex = 0;
    bool hasAdvanced = false;

    while(!hasAdvanced &&
          (localIndex < statesSequenceVector.size()))
    {
        Sequence* actualSeq = std::get<1>(statesSequenceVector[localIndex]);
        if(actualSeq->advanceSequence()) {
            hasAdvanced = true;
        } else {
            actualSeq->reset();
            localIndex++;
        }
    }
    maxSequenceIndex = std::max(maxSequenceIndex, localIndex);
}
