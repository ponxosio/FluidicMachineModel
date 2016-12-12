#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

#include "rules/arithmetic/arithmeticoperable.h"

class Variable : public ArithmeticOperable
{
public:
    Variable(const std::string & name);
    virtual ~Variable();

    virtual std::string translate();
protected:
    std::string name;
};

#endif // VARIABLE_H
