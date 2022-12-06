#ifndef POSTFIX_H
#define POSTFIX_H

#include <stack>
#include <string>
#include <vector>

class Postfix
{
public:
    Postfix();
    ~Postfix();

    void setInfix(std::string &infix);
    std::string getInfix() const;

    bool convert();

    std::string getPostfix() const;

private:
    std::string m_infix;
    std::string m_postfix;
    std::stack<std::string> m_stack;

    std::vector< std::vector< std::string> > m_operatorPrior; // operator precedence
    std::vector<std::string> m_operatorLst2; // list of operators with 2 chars
    std::vector<std::string> m_operatorLst1; // list of operators with 1 char

    std::string startsWithOperator(std::string &infix) const;
    int getOperatorPiority(std::string &operatr) const;
    std::string getOperand(std::string &infix) const;

};

#endif // POSTFIX_H
