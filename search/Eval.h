#ifndef EVAL_H
#define EVAL_H

#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

class Eval
{
public:
    Eval();
    ~Eval();

    void setPostfix(std::string &postfix);
    std::string getPostfix() const;

    bool evaluate();

    int getResult() const;

private:
    bool isOperand(std::string str) const;

    static int multiplication(int a, int b);
    static int division(int a, int b);
    static int remainder(int a, int b);
    static int addition(int a, int b);
    static int subtraction(int a, int b);
    static int bwLShift(int a, int b);
    static int bwRShift(int a, int b);
    static int less(int a, int b);
    static int lessEqual(int a, int b);
    static int greater(int a, int b);
    static int greaterEqual(int a, int b);
    static int equal(int a, int b);
    static int different(int a, int b);
    static int bwAnd(int a, int b);
    static int bwXor(int a, int b);
    static int bwOr(int a, int b);
    static int logAnd(int a, int b);
    static int logOr(int a, int b);

    enum ItemType{
        ITEM_OPERAND=0,
        ITEM_OPERATOR
    };
    typedef int(*funPtr)(int a, int b);
    union Data{
        constexpr Data() : operand{0} {};
        //~Data();
        int operand;
        funPtr operation;
    };
    struct Item{
        constexpr Item() : type{ITEM_OPERAND} {};
        //~Item();
        ItemType type;
        Eval::Data data;
    };


    std::string m_postfix;
    std::vector<Item> m_parsedPostfix;
    int m_result;
    std::stack<int> m_stack;
    std::unordered_map<std::string, funPtr> m_funMap;
};

#endif // EVAL_H
