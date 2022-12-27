#ifndef EXPRESSIONTREE_H
#define EXPRESSIONTREE_H

#include "TreeNode.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class ExpressionTree
{
public:
    ExpressionTree();
    ~ExpressionTree();

    void setVariable(const std::string &varName, const VariableNode::VariableInfo &varInfo);
    void setArray(const std::string &arrName, const ArrayNode::ArrayInfo &arrInfo);
    void clearVariablesMap();
    void clearArraysMap();

    bool build(const std::string &infix);
    std::string getInfix() const;

    int evaluate(bool &ok);
    std::string toString();

private:
    void clearStr(std::string& str);

    // top to down parser based on following BNF rules
    // expression ::= logOr_expression
    // logOr_expression ::= logAnd_expression { "||" logAnd_expression }
    // logAnd_expression ::= bwOr_expression { "&&" bwOr_expression }
    // bwOr_expression ::= bwXor_expression { "|" bwXor_expression }
    // bwXor_expression ::= bwAnd_expression { "^" bwAnd_expression }
    // bwAnd_expression ::= equality_expression { "&" equality_expression }
    // equality_expression ::= relational_expression { ("==" | "!=") relational_expression }
    // relational_expression ::= bwShift_expression { ("<" | "<=" | ">" | ">=") bwShift_expression }
    // bwShift_expression ::= add_expression { ("<<" | ">>") add_expression }
    // add_expression ::= mul_expression { ("+" | "-") mul_expression }
    // mul_expression ::= unary_expression { ("*" | "/" | "%") unary_expression }
    // unary_expression ::= ("+" | "-" | "!" | "~") unary_expression | primary
    // primary ::= VALUE | LPAREN expression RPAREN | ARRAY ID "[" expression "]" | VARIABLE ID
    std::unique_ptr<AbstractNode> expression();
    std::unique_ptr<AbstractNode> logOr_expression();
    std::unique_ptr<AbstractNode> logAnd_expression();
    std::unique_ptr<AbstractNode> bwOr_expression();
    std::unique_ptr<AbstractNode> bwXor_expression();
    std::unique_ptr<AbstractNode> bwAnd_expression();
    std::unique_ptr<AbstractNode> equality_expression();
    std::unique_ptr<AbstractNode> relational_expression();
    std::unique_ptr<AbstractNode> bwShift_expression();
    std::unique_ptr<AbstractNode> add_expression();
    std::unique_ptr<AbstractNode> mul_expression();
    std::unique_ptr<AbstractNode> unary_expression();
    std::unique_ptr<AbstractNode> primary();

    int matchOp(const std::vector<std::string> &whiteLst,
                const std::vector<std::string> &blackLst={}) const;
    int matchBinVal() const;
    int matchOctVal() const;
    int matchDecVal() const;
    int matchHexVal() const;
    int matchCharVal() const;
    int matchArray() const;
    int matchVar() const;

    int binStrToInt(const std::string str);
    int octStrToInt(const std::string str);
    int decStrToInt(const std::string str);
    int hexStrToInt(const std::string str);
    int charStrToInt(const std::string str);
    inline bool chInRange(const char ch, const char start, const char end) const;

    std::string consume(const int l);

    // unary operations
    static int uryPlus(int val, bool &ok);
    static int uryMinus(int val, bool &ok);
    static int uryLogNot(int val, bool &ok);
    static int uryBwNot(int val, bool &ok);

    // binary operations
    static int multiplication(int left, int right, bool &ok);
    static int division(int left, int right, bool &ok);
    static int remainder(int left, int right, bool &ok);
    static int addition(int left, int right, bool &ok);
    static int subtraction(int left, int right, bool &ok);
    static int bwLShift(int left, int right, bool &ok);
    static int bwRShift(int left, int right, bool &ok);
    static int less(int left, int right, bool &ok);
    static int lessEqual(int left, int right, bool &ok);
    static int greater(int left, int right, bool &ok);
    static int greaterEqual(int left, int right, bool &ok);
    static int equal(int left, int right, bool &ok);
    static int different(int left, int right, bool &ok);
    static int bwAnd(int left, int right, bool &ok);
    static int bwXor(int left, int right, bool &ok);
    static int bwOr(int left, int right, bool &ok);
    static int logAnd(int left, int right, bool &ok);
    static int logOr(int left, int right, bool &ok);

    std::string m_infix;
    int m_idx;
    std::unique_ptr<TreeRoot> m_tree;

    std::unordered_map<std::string, char> m_scapeMap;

    std::unordered_map<std::string, VariableNode::VariableInfo> m_varsMap;
    std::unordered_map<std::string, ArrayNode::ArrayInfo> m_arrsMap;

    typedef int(*uryFunPtr)(int, bool&); // function pointer for unary operations
    typedef int(*binFunPtr)(int, int, bool&); // function pointer for binary operations
    std::unordered_map<std::string, uryFunPtr> m_uryOperationMap;
    std::unordered_map<std::string, binFunPtr> m_binOperationMap;
};

#endif // EXPRESSIONTREE_H
