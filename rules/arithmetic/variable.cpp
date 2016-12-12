#include "variable.h"

Variable::Variable(const std::string & name) :
    ArithmeticOperable()
{
    this->name = std::string(name);
}

Variable::~Variable() {

}

std::string Variable::translate() {
    return name;
}
