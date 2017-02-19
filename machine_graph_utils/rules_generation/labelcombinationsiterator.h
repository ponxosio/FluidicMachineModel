#ifndef ZEROCOMBINATIONSITERATOR_H
#define ZEROCOMBINATIONSITERATOR_H

#include <vector>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include <machine_graph_utils/rules_generation/label.h>
#include <util/sequence.h>
#include <util/Utils.h>

#include "fluidicmachinemodel_global.h"

class LABELCOMBINATIONITERATOR_EXPORT LabelCombinationsIterator
{
public:
    typedef std::unordered_map<std::tuple<int,int>, Label, PairIntIntHashFunction> LabelTypeTubeMap;
    typedef std::unordered_set<Label, Label::LabelHash> LabelSet;

    LabelCombinationsIterator(const LabelTypeTubeMap  & labelMap, const std::vector<Label::LabelType> & types);
    virtual ~LabelCombinationsIterator();

    bool calculateNext();

    inline const LabelTypeTubeMap & getTubesLabelsMap() {
        return actualTubesLabels;
    }
    inline const LabelSet & getLabelSet() {
        return actualLabelSet;
    }

private:
    int maxSequenceIndex;
    std::vector<std::tuple<std::tuple<int, int>,Sequence*>> statesSequenceVector;
    LabelTypeTubeMap actualTubesLabels;
    LabelSet actualLabelSet;

    void advanceSequence();
    void fillActualMap();
};

#endif // ZEROCOMBINATIONSITERATOR_H
