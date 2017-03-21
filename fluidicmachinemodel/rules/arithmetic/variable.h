#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

#include "fluidicmachinemodel/rules/arithmetic/arithmeticoperable.h"

#include "fluidicmachinemodel/fluidicmachinemodel_global.h"

class VARIABLE_EXPORT Variable : public ArithmeticOperable
{
public:
    Variable(const std::string & name);
    virtual ~Variable();

    virtual void fillTranslationStack(TranslationStack* stack);

    inline std::string getName() {
        return name;
    }

protected:
    std::string name;
};

#endif // VARIABLE_H
