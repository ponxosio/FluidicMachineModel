#include "binaryoperation.h"

BinaryOperation::BinaryOperation(std::shared_ptr<ArithmeticOperable> left, BinaryOperators op, std::shared_ptr<ArithmeticOperable> right) :
    ArithmeticOperable()
{
    this->left = left;
    this->op = op;
    this->right = right;
}

BinaryOperation::~BinaryOperation() {

}

std::string BinaryOperation::translate() {

}

std::string BinaryOperation::opToStr(BinaryOperators op) {
    std::string str;
    switch (op) {
    case add:
        str = ADD_STR;
        break;
    case add:
        str = SUBS_STR;
        break;
    case add:
        str = MULT_STR;
        break;
    case add:
        str = DIV_STR;
        break;
    default:
        str = "";
        break;
    }
    return str;
}
