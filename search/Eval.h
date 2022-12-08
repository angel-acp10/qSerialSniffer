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

    enum VarType {
        TYPE_UINT8=0,
        TYPE_UINT16,
        TYPE_UINT32,
        TYPE_UINT64
    };

    void clearArrayMap();
    void addToArrayMap(const std::string &arrayName, void * const arrayPtr,
                       const VarType type, const int maxLength);

    void setPostfix(const std::string &postfix);
    std::string getPostfix() const;

    bool evaluate();

    int getResult() const;

private:
    struct ArrayInfo {
        void *ptr;
        VarType varType;
        int maxLength;
    };

    enum PostfixItemType{
        ITEM_OPERAND=0,
        ITEM_ARRAY,
        ITEM_OPERATOR
    };

    struct Item{
        constexpr Item() : type{ITEM_OPERAND} {};
        //~Item();

        PostfixItemType type;
        union Data
        {
            constexpr Data() : operand{0} {};
            //~Data();
            int operand;        // ITEM_OPERAND
            ArrayInfo array;    // ITEM_ARRAY
            int(*operation)(const Item &a, const Item &b, bool &ok);   // ITEM_OPERATOR
        }data;
    };

    typedef int(*funPtr)(const Item &a, const Item &b, bool &ok);


    bool isOperand(const std::string &str) const;
    bool isArray(const std::string &str, Eval::ArrayInfo &array) const;

    static int arraySubscript(const Item &a, const Item &b, bool &ok);
    static int multiplication(const Item &a, const Item &b, bool &ok);
    static int division(const Item &a, const Item &b, bool &ok);
    static int remainder(const Item &a, const Item &b, bool &ok);
    static int addition(const Item &a, const Item &b, bool &ok);
    static int subtraction(const Item &a, const Item &b, bool &ok);
    static int bwLShift(const Item &a, const Item &b, bool &ok);
    static int bwRShift(const Item &a, const Item &b, bool &ok);
    static int less(const Item &a, const Item &b, bool &ok);
    static int lessEqual(const Item &a, const Item &b, bool &ok);
    static int greater(const Item &a, const Item &b, bool &ok);
    static int greaterEqual(const Item &a, const Item &b, bool &ok);
    static int equal(const Item &a, const Item &b, bool &ok);
    static int different(const Item &a, const Item &b, bool &ok);
    static int bwAnd(const Item &a, const Item &b, bool &ok);
    static int bwXor(const Item &a, const Item &b, bool &ok);
    static int bwOr(const Item &a, const Item &b, bool &ok);
    static int logAnd(const Item &a, const Item &b, bool &ok);
    static int logOr(const Item &a, const Item &b, bool &ok);

    std::unordered_map<std::string, ArrayInfo> m_arrMap;
    std::string m_postfix;
    std::vector<Item> m_parsedPostfix; // it can be ITEM_OPERAND, ITEM_ARRAY, ITEM_OPERATOR
    int m_result;
    std::stack<Item> m_stack; // it can only be ITEM_OPERAND, ITEM_ARRAY
    std::unordered_map<std::string, funPtr> m_funMap;
};

#endif // EVAL_H
