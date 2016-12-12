#ifndef RULE_H
#define RULE_H

class Rule
{

public:
    Rule(){}
    virtual ~Rule(){}

    virtual void generateRestriction() = 0;
};

#endif // RULE_H
