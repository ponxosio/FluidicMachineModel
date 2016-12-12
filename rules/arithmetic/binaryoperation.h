#ifndef BINARYOPERATION_H
#define BINARYOPERATION_H

#include <string>
#include <memory>

#include "rules/arithmetic/arithmeticoperable.h"

#define ADD_STR "+"
#define SUBS_STR "-"
#define MULT_STR "*"
#define DIV_STR "/"

typedef enum BinaryOperators_ {
    add,
    subtract,
    multiply,
    divide
} BinaryOperators;

class BinaryOperation : public ArithmeticOperable
{
public:
    BinaryOperation(std::shared_ptr<ArithmeticOperable> left, BinaryOperators op, std::shared_ptr<ArithmeticOperable> right);
    virtual ~BinaryOperation();

    virtual std::string translate();

    inline std::shared_ptr<ArithmeticOperable> operator+(std::shared_ptr<ArithmeticOperable> left, std::shared_ptr<ArithmeticOperable> right) {
        std::shared_ptr<ArithmeticOperable> binaryOp = std::shared_ptr<ArithmeticOperable>(new BinaryOperation(left, add, right));
        return binaryOp;
    }

    inline std::shared_ptr<ArithmeticOperable> operator-(std::shared_ptr<ArithmeticOperable> left, std::shared_ptr<ArithmeticOperable> right) {
        std::shared_ptr<ArithmeticOperable> binaryOp = std::shared_ptr<ArithmeticOperable>(new BinaryOperation(left, subtract, right));
        return binaryOp;
    }

    inline std::shared_ptr<ArithmeticOperable> operator*(std::shared_ptr<ArithmeticOperable> left, std::shared_ptr<ArithmeticOperable> right) {
        std::shared_ptr<ArithmeticOperable> binaryOp = std::shared_ptr<ArithmeticOperable>(new BinaryOperation(left, multiply, right));
        return binaryOp;
    }

    inline std::shared_ptr<ArithmeticOperable> operator/(std::shared_ptr<ArithmeticOperable> left, std::shared_ptr<ArithmeticOperable> right) {
        std::shared_ptr<ArithmeticOperable> binaryOp = std::shared_ptr<ArithmeticOperable>(new BinaryOperation(left, divide, right));
        return binaryOp;
    }
protected:
    std::shared_ptr<ArithmeticOperable> left;
    BinaryOperators op;
    std::shared_ptr<ArithmeticOperable> right;

    std::string opToStr(BinaryOperators op);
};

#endif // BINARYOPERATION_H
