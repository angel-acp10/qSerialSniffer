#include "ExpressionTree.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

ExpressionTree::ExpressionTree()
      : m_infix(""),
        m_tree(nullptr),
        m_scapeMap({
            {"'\\a'", '\a'},
            {"'\\b'", '\b'},
            {"'\\e'", '\e'},
            {"'\\f'", '\f'},
            {"'\\n'", '\n'},
            {"'\\r'", '\r'},
            {"'\\t'", '\t'},
            {"'\\v'", '\v'},
            {"'\\\\'", '\\'},
            {"'\\''", '\''},
            {"'\\?'", '\?'}
        }),
        m_varsMap(),
        m_arrsMap(),
        m_uryOperationMap({
            {"+", &ExpressionTree::uryPlus},
            {"-", &ExpressionTree::uryMinus},
            {"!", &ExpressionTree::uryLogNot},
            {"~", &ExpressionTree::uryBwNot},
        }),
        m_binOperationMap({
            {"*", &ExpressionTree::multiplication},
            {"/", &ExpressionTree::division},
            {"%", &ExpressionTree::remainder},
            {"+", &ExpressionTree::addition},
            {"-", &ExpressionTree::subtraction},
            {"<<", &ExpressionTree::bwLShift},
            {">>", &ExpressionTree::bwRShift},
            {"<", &ExpressionTree::less},
            {"<=", &ExpressionTree::lessEqual},
            {">", &ExpressionTree::greater},
            {">=", &ExpressionTree::greaterEqual},
            {"==", &ExpressionTree::equal},
            {"!=", &ExpressionTree::different},
            {"&", &ExpressionTree::bwAnd},
            {"^", &ExpressionTree::bwXor},
            {"|", &ExpressionTree::bwOr},
            {"&&", &ExpressionTree::logAnd},
            {"||", &ExpressionTree::logOr},
        })
{
}
ExpressionTree::~ExpressionTree()
{
}

void ExpressionTree::setVariable(const std::string &varName, const VariableNode::VariableInfo &varInfo)
{
    m_varsMap[varName] = varInfo;
}
void ExpressionTree::setArray(const std::string &arrName, const ArrayNode::ArrayInfo &arrInfo)
{
    m_arrsMap[arrName] = arrInfo;
}
void ExpressionTree::clearVariablesMap()
{
    m_varsMap.clear();
}
void ExpressionTree::clearArraysMap()
{
    m_arrsMap.clear();
}

bool ExpressionTree::build(const std::string &infix)
{
    m_infix = infix;
    clearStr(m_infix);
    m_idx = 0;
    m_tree.reset(new TreeRoot(expression()));
    return true;
}

std::string ExpressionTree::getInfix() const
{
    return m_infix;
}

int ExpressionTree::evaluate(bool &ok)
{
    if(m_tree == nullptr)
    {
        ok = false;
        return -1;
    }
    int res = m_tree->eval(ok);
    if(!ok)
    {
        ok = false;
        return res;
    }
    ok = true;
    return res;
}

std::string ExpressionTree::toString()
{
    return m_tree->toString();
}

void ExpressionTree::clearStr(std::string& str)
{
    auto removeCondition = [](const char ch) -> bool
    {
        if(ch=='\n' || ch == '\r' || ch == ' ')
            return true;
        return false;
    };
    str.erase(std::remove_if(str.begin(), str.end(), removeCondition), str.cend());
}

std::unique_ptr<AbstractNode> ExpressionTree::expression()
{
    return logOr_expression();
}

std::unique_ptr<AbstractNode> ExpressionTree::logOr_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = logAnd_expression();
    while( (l = matchOp({"||"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = logAnd_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::logAnd_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = bwOr_expression();
    while( (l = matchOp({"&&"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = bwOr_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::bwOr_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = bwXor_expression();
    while( (l = matchOp({"|"},{"||"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = bwXor_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::bwXor_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = bwAnd_expression();
    while( (l = matchOp({"^"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = bwAnd_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::bwAnd_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = equality_expression();
    while( (l = matchOp({"&"},{"&&"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = equality_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::equality_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = relational_expression();
    while( (l = matchOp({"==","!="})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = relational_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::relational_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = bwShift_expression();
    while( (l = matchOp({"<=",">=","<",">"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = bwShift_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::bwShift_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = add_expression();
    while( (l = matchOp({"<<",">>"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = add_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::add_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = mul_expression();
    while( (l = matchOp({"+","-"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = mul_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::mul_expression()
{
    int l = 0;
    std::unique_ptr<AbstractNode> left = unary_expression();
    while( (l = matchOp({"*","/","%"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> right = unary_expression();
        left.reset(new BOperatorNode(op,
                                     m_binOperationMap[op],
                                     std::move(left),
                                     std::move(right)
                                     )
                    );
    }
    return left;
}
std::unique_ptr<AbstractNode> ExpressionTree::unary_expression()
{
    int l = 0;
    if( (l = matchOp({"+","-","!","~"})) )
    {
        std::string op = consume(l);
        std::unique_ptr<AbstractNode> inner = unary_expression();
        inner.reset(new UOperatorNode(op,
                                     m_uryOperationMap[op],
                                     std::move(inner)
                                     )
                    );
        return inner;
    }
    else
        return primary();
}
std::unique_ptr<AbstractNode> ExpressionTree::primary()
{
    int l = 0;
    int value;
    if( (l=matchHexVal()) )             value = hexStrToInt(consume(l));
    else if( (l=matchBinVal()) )        value = binStrToInt(consume(l));
    else if( (l=matchOctVal()) )        value = octStrToInt(consume(l));
    else if( (l=matchDecVal()) )        value = decStrToInt(consume(l));
    else if( (l=matchCharVal()) )       value = charStrToInt(consume(l));
    if(l) // it was a constant value
    {
        std::unique_ptr<AbstractNode> number(new NumberNode(value));
        return number;
    }
    else if( (l=matchOp({"("})) )
    {
        consume(l); // (
        std::unique_ptr<AbstractNode> inner = expression();
        std::unique_ptr<AbstractNode> parentheses(new ParenthesesNode(std::move(inner)));
        consume(1); // )
        return parentheses;
    }
    else if( (l=matchArray()) )
    {
        std::string name = consume(l); //takes the name of the array
        consume(1); // [
        std::unique_ptr<AbstractNode> arrIdx = expression();
        std::unique_ptr<AbstractNode> array(new ArrayNode(name,
                                                          &m_arrsMap,
                                                          std::move(arrIdx))
                                            );
        consume(1); // ]
        return array;
    }
    else if( (l=matchVar()) )
    {
        std::string name = consume(l);
        std::unique_ptr<AbstractNode> variable(new VariableNode(name, &m_varsMap));
        return variable;
    }
    std::unique_ptr<AbstractNode> null = nullptr;
    return null;
}

int ExpressionTree::matchOp(const std::vector<std::string> &whiteLst,
                            const std::vector<std::string> &blackLst) const
{
    for(const auto &op : blackLst)
    {
        if(m_infix.compare(m_idx, op.length(), op) == 0)
            return 0;
    }
    for(const auto &op : whiteLst)
    {
        if(m_infix.compare(m_idx, op.length(), op) == 0)
            return op.length();
    }
    return 0;
}

int ExpressionTree::matchBinVal() const
{
    const std::string& str = m_infix;
    int i = m_idx;
    int strLen = static_cast<int>(str.length());

    if(str.compare(i,2,"0b")==0 || str.compare(i,2,"0B")==0)
    {
        int numLen = 2;
        i += 2;
        while( i<strLen && chInRange(str[i],'0','1') )
        {
            numLen++;
            i++;
        }
        return numLen;
    }
    return 0;
}

int ExpressionTree::matchOctVal() const
{
    const std::string& str = m_infix;
    int i = m_idx;
    int strLen = static_cast<int>(str.length());

    if(str.compare(i,1,"0")==0)
    {
        int numLen = 1;
        i += 1;
        while( i<strLen && chInRange(str[i],'0','7') )
        {
            numLen++;
            i++;
        }
        if(chInRange(str[i],'8','9')) // out of octal range
            return 0;
        return numLen;
    }
    return 0;
}

int ExpressionTree::matchDecVal() const
{
    const std::string& str = m_infix;
    int i = m_idx;
    int strLen = static_cast<int>(str.length());

    int numLen = 0;
    while( i<strLen && chInRange(str[i],'0','9') )
    {
        numLen++;
        i++;
    }
    return numLen;
}

int ExpressionTree::matchHexVal() const
{
    const std::string& str = m_infix;
    int i = m_idx;
    int strLen = static_cast<int>(str.length());

    if(str.compare(i,2,"0x")==0 || str.compare(i,2,"0X")==0)
    {
        int numLen = 2;
        i += 2;
        while( i<strLen &&
                (chInRange(str[i],'0','9')
                || chInRange(str[i],'A','F')
                || chInRange(str[i],'a','f')) )
        {
            numLen++;
            i++;
        }
        return numLen;
    }
    return 0;
}

int ExpressionTree::matchCharVal() const
{
    const std::string& str = m_infix;
    int i = m_idx;
    int strLen = static_cast<int>(str.length());

    if(str[i] == '\'' && i+3<strLen && str[i+3] == '\'')
    { // char (with scaped notation)
        auto search = m_scapeMap.find(str.substr(i,4));
        if(search != m_scapeMap.end())
            return 4; // scaped char exists
    }
    else if(str[i] == '\'' && i+2<strLen && str[i+2] == '\'')
    { // char
        return 3;
    }
    return 0;
}


int ExpressionTree::matchArray() const
{
    const std::string& str = m_infix;
    int i = m_idx;
    int strLen = static_cast<int>(str.length());

    if(chInRange(str[i],'A','Z') || chInRange(str[i],'a','z') || str[i]=='_')
    {
        int numLen = 1;
        i += 1;
        while( i<strLen &&
               (chInRange(str[i],'0','9')
                || chInRange(str[i],'A','Z')
                || chInRange(str[i],'a','z')
                || str[i]=='_') )
        {
            numLen++;
            i++;
        }
        if(str[i]=='[')
            return numLen;
    }
    return 0;
}


int ExpressionTree::matchVar() const
{
    const std::string& str = m_infix;
    int i = m_idx;
    int strLen = static_cast<int>(str.length());

    if(chInRange(str[i],'A','Z') || chInRange(str[i],'a','z') || str[i]=='_')
    {
        int numLen = 1;
        i += 1;
        while( i<strLen &&
               (chInRange(str[i],'0','9')
                || chInRange(str[i],'A','Z')
                || chInRange(str[i],'a','z')
                || str[i]=='_') )
        {
            numLen++;
            i++;
        }
        return numLen;
    }
    return 0;
}

int ExpressionTree::binStrToInt(const std::string str)
{
    try
    {
        return std::stoi(&str[2], nullptr, 2); // handles oct, dec and hex
    }
    catch (std::invalid_argument const& ex)
    {
        std::cout << ex.what() << ": failed converting \"" << str << "\"" << '\n';
        return -1;
    }
}

int ExpressionTree::octStrToInt(const std::string str)
{
    try
    {
        return stoi(str, nullptr, 8);
    }
    catch (std::invalid_argument const& ex)
    {
        std::cout << ex.what() << ": failed converting \"" << str << "\"" << '\n';
        return -1;
    }
}

int ExpressionTree::decStrToInt(const std::string str)
{
    try
    {
        return stoi(str, nullptr, 10);
    }
    catch (std::invalid_argument const& ex)
    {
        std::cout << ex.what() << ": failed converting \"" << str << "\"" << '\n';
        return -1;
    }
}

int ExpressionTree::hexStrToInt(const std::string str)
{
    try
    {
        return stoi(str, nullptr, 16); // handles oct, dec and hex
    }
    catch (std::invalid_argument const& ex)
    {
        std::cout << ex.what() << ": failed converting \"" << str << "\"" << '\n';
        return -1;
    }
}

int ExpressionTree::charStrToInt(const std::string str)
{
    // is it a scape character?
    auto search = m_scapeMap.find(str);
    if(search != m_scapeMap.end())
        return static_cast<int>(search->second);

    // is it a character?
    else if(str.front()=='\'' && str.back()=='\'' && str.length()==3)
        return static_cast<int>(str[1]);

    return -1;
}

inline bool ExpressionTree::chInRange(const char ch, const char start, const char end) const
{
    if(ch >= start  &&  ch <= end)
        return true;
    return false;
}

std::string ExpressionTree::consume(int l)
{
    std::string res = m_infix.substr(m_idx, l);
    m_idx += l;
    return res;
}

// unary operations
int ExpressionTree::uryPlus(int val, bool &ok)
{
    ok = true;
    return val;
}
int ExpressionTree::uryMinus(int val, bool &ok)
{
    ok = true;
    return -val;
}
int ExpressionTree::uryLogNot(int val, bool &ok)
{
    ok = true;
    return !val;
}
int ExpressionTree::uryBwNot(int val, bool &ok)
{
    ok = true;
    return ~val;
}

// binary operations
int ExpressionTree::multiplication(int left, int right, bool &ok)
{
    ok = true;
    return left * right;
}
int ExpressionTree::division(int left, int right, bool &ok)
{
    ok = true;
    if(right == 0)
    {
        ok = false;
        return -1; // cannot divide by 0
    }
    return left / right;
}
int ExpressionTree::remainder(int left, int right, bool &ok)
{
    ok = true;
    if(right == 0)
    {
        ok = false;
        return -1; // cannot divide by 0
    }
    return left % right;
}
int ExpressionTree::addition(int left, int right, bool &ok)
{
    ok = true;
    return left + right;
}
int ExpressionTree::subtraction(int left, int right, bool &ok)
{
    ok = true;
    return left - right;
}
int ExpressionTree::bwLShift(int left, int right, bool &ok)
{
    ok = true;
    if(right < 0)
    {
        ok = false;
        return -1;
    }
    return left << right;
}
int ExpressionTree::bwRShift(int left, int right, bool &ok)
{
    ok = true;
    if(right < 0)
    {
        ok = false;
        return -1;
    }
    return left >> right;
}
int ExpressionTree::less(int left, int right, bool &ok)
{
    ok = true;
    return left < right;
}
int ExpressionTree::lessEqual(int left, int right, bool &ok)
{
    ok = true;
    return left <= right;
}
int ExpressionTree::greater(int left, int right, bool &ok)
{
    ok = true;
    return left > right;
}
int ExpressionTree::greaterEqual(int left, int right, bool &ok)
{
    ok = true;
    return left >= right;
}
int ExpressionTree::equal(int left, int right, bool &ok)
{
    ok = true;
    return left == right;
}
int ExpressionTree::different(int left, int right, bool &ok)
{
    ok = true;
    return left != right;
}
int ExpressionTree::bwAnd(int left, int right, bool &ok)
{
    ok = true;
    return left & right;
}
int ExpressionTree::bwXor(int left, int right, bool &ok)
{
    ok = true;
    return left ^ right;
}
int ExpressionTree::bwOr(int left, int right, bool &ok)
{
    ok = true;
    return left | right;
}
int ExpressionTree::logAnd(int left, int right, bool &ok)
{
    ok = true;
    return left && right;
}
int ExpressionTree::logOr(int left, int right, bool &ok)
{
    ok = true;
    return left || right;
}

