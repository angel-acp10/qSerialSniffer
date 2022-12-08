#ifndef EVAL_H
#define EVAL_H

#include <stack>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

class Eval
{
public:
    Eval();
    ~Eval();

    void clearArrayMap();
    void addToArrayMap(const std::string &arrayName, int *arrayPtr);

    void setPostfix(std::string &postfix);
    std::string getPostfix() const;

    bool evaluate();

    int getResult() const;

private:
    bool isOperand(std::string str) const;
    int* isArray(std::string str) const;

    static int arraySubscript(int *a, int b);
    static int multiplication(int *a, int b);
    static int division(int *a, int b);
    static int remainder(int *a, int b);
    static int addition(int *a, int b);
    static int subtraction(int *a, int b);
    static int bwLShift(int *a, int b);
    static int bwRShift(int *a, int b);
    static int less(int *a, int b);
    static int lessEqual(int *a, int b);
    static int greater(int *a, int b);
    static int greaterEqual(int *a, int b);
    static int equal(int *a, int b);
    static int different(int *a, int b);
    static int bwAnd(int *a, int b);
    static int bwXor(int *a, int b);
    static int bwOr(int *a, int b);
    static int logAnd(int *a, int b);
    static int logOr(int *a, int b);

    enum PostfixItemType{
        ITEM_OPERAND=0,
        ITEM_ARR_PTR,
        ITEM_OPERATOR
    };
    typedef int(*funPtr)(int *a, int b);
    union Data{
        constexpr Data() : operand{0} {};
        //~Data();
        int operand;        // ITEM_OPERAND
        int *arrPtr;        // ITEM_ARR_PTR
        funPtr operation;   // ITEM_OPERATOR

    };
    struct Item{
        constexpr Item() : type{ITEM_OPERAND} {};
        //~Item();
        PostfixItemType type;
        Eval::Data data;
    };


    std::unordered_map<std::string, int*> m_arrMap;
    std::string m_postfix;
    std::vector<Item> m_parsedPostfix; // it can be ITEM_OPERAND, ITEM_ARR_PTR, ITEM_OPERATOR
    int m_result;
    std::stack<Item> m_stack; // it can only be ITEM_OPERAND, ITEM_ARR_PTR
    std::unordered_map<std::string, funPtr> m_funMap;
};

#endif // EVAL_H
