#ifndef ARITHMETICOPERABLE_H
#define ARITHMETICOPERABLE_H

#include <string>

class ArithmeticOperable
{

public:
    ArithmeticOperable(){}
    virtual ~ArithmeticOperable(){}

    virtual std::string translate() = 0;
};

#endif // ARITHMETICOPERABLE_H
