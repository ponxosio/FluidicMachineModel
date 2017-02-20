#ifndef LABEL_H
#define LABEL_H

#include <bitset>

class Label {
public:

    typedef enum LabelType_ {
        zero = 1,
        bigger = 2,
        smaller = 4,
    } LabelType;

    typedef struct {
        size_t operator()(Label::LabelType type) const throw () {
            int value = std::floor(((int) type)/2.0);
            return value;
        }
    } LabelTypeHash;

    typedef struct {
        size_t operator()(const Label & label) const throw () {
            return label.labelSet.to_ulong();
        }
    } LabelHash;

    Label() {}
    Label(const Label & l) : labelSet(l.labelSet) {}
    virtual ~Label() {}

    inline bool onlyType(Label::LabelType & type) {
        if (labelSet.count() == 1) {
            type = (Label::LabelType) labelSet.to_ulong();
            return true;
        } else {
            return false;
        }
    }

    inline void setLabel(LabelType type) {
        int id = std::floor(((int)type)/2.0);
        labelSet.set(id, 1);
    }

    inline bool isDummy() {
        return labelSet.none();
    }

    inline bool isFull() {
        return labelSet.all();
    }

    inline bool hasLabelMask(unsigned long mask) {
        return ((mask & labelSet.to_ulong()) == mask);
    }

    inline unsigned long getMask() {
        return (labelSet.to_ullong());
    }

    inline std::string toStr() {
        return labelSet.to_string();
    }

    inline bool operator==(const Label & lhs) const {
        return (this->labelSet == lhs.labelSet);
    }

protected:
    std::bitset<3> labelSet;
};


#endif // LABEL_H
