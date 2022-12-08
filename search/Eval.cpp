#include "Eval.h"
#include <sstream>

Eval::Eval()
    : m_postfix(""),
      m_stack(),
      m_funMap()
{
    m_funMap = {
        {"[]", &Eval::arraySubscript},
        {"*", &Eval::multiplication},
        {"/", &Eval::division},
        {"%", &Eval::remainder},
        {"+", &Eval::addition},
        {"-", &Eval::subtraction},
        {"<<", &Eval::bwLShift},
        {">>", &Eval::bwRShift},
        {"<", &Eval::less},
        {"<=", &Eval::lessEqual},
        {">", &Eval::greater},
        {">=", &Eval::greaterEqual},
        {"==", &Eval::equal},
        {"!=", &Eval::different},
        {"&", &Eval::bwAnd},
        {"^", &Eval::bwXor},
        {"|", &Eval::bwOr},
        {"&&", &Eval::logAnd},
        {"||", &Eval::logOr},
    };
}

Eval::~Eval()
{
}

void Eval::clearArrayMap()
{
    m_arrMap.clear();
}

void Eval::addToArrayMap(const std::string &arrayName, int *arrayPtr)
{
    m_arrMap[arrayName] = arrayPtr;
}

void Eval::setPostfix(std::string &postfix)
{
    if(m_postfix == postfix)
        return;

    m_postfix = postfix;
    m_parsedPostfix.clear();
    std::istringstream p(postfix);
    std::string str;

    while(std::getline(p, str, ' '))
    {
        Item item;

        if(str == "")
            continue;

        else if(isOperand(str))
        {
            int *arrPtr = isArray(str);
            if(arrPtr == nullptr)
            {
                item.type = ITEM_OPERAND;
                item.data.operand = std::stoi(str);
                m_parsedPostfix.push_back(item);
            }
            else
            {
                item.type = ITEM_ARR_PTR;
                item.data.arrPtr = arrPtr;
                m_parsedPostfix.push_back(item);
            }
        }
        else
        {
            item.type = ITEM_OPERATOR;
            item.data.operation = m_funMap[str];
            m_parsedPostfix.push_back(item);
        }
    }
}

std::string Eval::getPostfix() const
{
    return m_postfix;
}

bool Eval::evaluate()
{
    // init variables
    m_result = 0;
    std::string postfix = m_postfix;
    while(!m_stack.empty())
    {
        m_stack.pop();
    }

    // start algorithm
    Item a, b, res;
    for(auto &item : m_parsedPostfix)
    {
        switch(item.type)
        {
        case ITEM_OPERAND:
            m_stack.push(item);
            break;

        case ITEM_ARR_PTR:
            m_stack.push(item);
            break;

        case ITEM_OPERATOR:
            b = m_stack.top();
            m_stack.pop();

            a = m_stack.top();
            m_stack.pop();

            if(item.data.operation == &arraySubscript)
            {
                res.type = ITEM_OPERAND;
                res.data.operand = item.data.operation(a.data.arrPtr, b.data.operand);
            }
            else
            {
                res.type = ITEM_OPERAND;
                res.data.operand = item.data.operation(&a.data.operand, b.data.operand);
            }
            m_stack.push( res );

            break;

        default:
            break;
        }
    }
    m_result = m_stack.top().data.operand;
    m_stack.pop();
    return true;
}

int Eval::getResult() const
{
    return m_result;
}

bool Eval::isOperand(std::string str) const
{
    for(auto ch : str)
    {
        if( (ch>='0' && ch<='9') || (ch>='A' && ch<='Z') ||
            (ch>='a' && ch<='z') ||  ch=='_' )
        {
            continue;
        }
        else
            return false;
    }
    return true;
}

int* Eval::isArray(std::string str) const
{
    auto search = m_arrMap.find(str);
    if(search == m_arrMap.end())
        return nullptr;

    return search->second;
}

// used by funPtr
int Eval::arraySubscript(int *a, int b) { return a[b]; }
int Eval::multiplication(int *a, int b)  { return (*a)*b; }
int Eval::division(int *a, int b)    { return (*a)/b; }
int Eval::remainder(int *a, int b)   { return (*a)%b; }
int Eval::addition(int *a, int b)    { return (*a)+b; }
int Eval::subtraction(int *a, int b) { return (*a)-b; }
int Eval::bwLShift(int *a, int b)    { return (*a)<<b; }
int Eval::bwRShift(int *a, int b)    { return (*a)>>b; }
int Eval::less(int *a, int b)        { return (*a)<b; }
int Eval::lessEqual(int *a, int b)   { return (*a)<=b; }
int Eval::greater(int *a, int b)     { return (*a)>b; }
int Eval::greaterEqual(int *a, int b){ return (*a)>=b; }
int Eval::equal(int *a, int b)       { return (*a)==b; }
int Eval::different(int *a, int b)   { return (*a)!=b; }
int Eval::bwAnd(int *a, int b)   { return (*a)&b; }
int Eval::bwXor(int *a, int b)   { return (*a)^b; }
int Eval::bwOr(int *a, int b)    { return (*a)|b; }
int Eval::logAnd(int *a, int b)  { return (*a)&&b; }
int Eval::logOr(int *a, int b)   { return (*a)||b; }
