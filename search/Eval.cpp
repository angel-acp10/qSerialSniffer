#include "Eval.h"
#include <sstream>
#include <bit>

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

void Eval::addToArrayMap(const std::string &arrayName, void * const arrayPtr,
                         const VarType varType, const int maxLength)
{
    m_arrMap[arrayName] = {
        .ptr = arrayPtr,
        .varType = varType,
        .maxLength = maxLength
    };
}

void Eval::setPostfix(const std::string &postfix)
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
            ArrayInfo array;
            if(isArray(str, array))
            {
                item.type = ITEM_ARRAY;
                item.data.array = array;
                m_parsedPostfix.push_back(item);
            }
            else
            {
                item.type = ITEM_OPERAND;
                item.data.operand = std::stoi(str);
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
    bool ok;
    for(auto &item : m_parsedPostfix)
    {
        switch(item.type)
        {
        case ITEM_OPERAND:
            m_stack.push(item);
            break;

        case ITEM_ARRAY:
            m_stack.push(item);
            break;

        case ITEM_OPERATOR:
            b = m_stack.top();
            m_stack.pop();

            a = m_stack.top();
            m_stack.pop();

            res.type = ITEM_OPERAND;
            res.data.operand = item.data.operation(a, b, ok);
            if(ok == false)
                return false;
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

bool Eval::isOperand(const std::string &str) const
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

bool Eval::isArray(const std::string &str, Eval::ArrayInfo &array) const
{
    auto search = m_arrMap.find(str);
    if(search == m_arrMap.end())
        return false;

    array = search->second;
    return true;
}

// used by funPtr
int Eval::arraySubscript(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if(a.type != ITEM_ARRAY)
    {
        ok = false;
        return -1;
    }
    else if((b.data.operand < 0) || (b.data.operand >= a.data.array.maxLength))
    {
        ok = false;
        return -2;
    }

    switch(a.data.array.varType)
    {
    case TYPE_UINT8:    return (int)( (uint8_t*)a.data.array.ptr)[b.data.operand];
    case TYPE_UINT16:   return (int)((uint16_t*)a.data.array.ptr)[b.data.operand];
    case TYPE_UINT32:   return (int)((uint32_t*)a.data.array.ptr)[b.data.operand];
    case TYPE_UINT64:   return (int)((uint64_t*)a.data.array.ptr)[b.data.operand];
    }
}

int Eval::multiplication(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand * b.data.operand;
}
int Eval::division(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand / b.data.operand;
}
int Eval::remainder(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand % b.data.operand;
}
int Eval::addition(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand + b.data.operand;
}
int Eval::subtraction(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand - b.data.operand;
}
int Eval::bwLShift(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    if(b.data.operand < 0)
    {
        ok = false;
        return -2;
    }
    return a.data.operand << b.data.operand;
}
int Eval::bwRShift(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    if(b.data.operand < 0)
    {
        ok = false;
        return -2;
    }
    return a.data.operand >> b.data.operand;
}
int Eval::less(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand < b.data.operand;
}
int Eval::lessEqual(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand <= b.data.operand;
}
int Eval::greater(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand > b.data.operand;
}
int Eval::greaterEqual(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand >= b.data.operand;
}
int Eval::equal(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand == b.data.operand;
}
int Eval::different(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand != b.data.operand;
}
int Eval::bwAnd(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand & b.data.operand;
}
int Eval::bwXor(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand ^ b.data.operand;
}
int Eval::bwOr(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand | b.data.operand;
}
int Eval::logAnd(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand && b.data.operand;
}
int Eval::logOr(const Item &a, const Item &b, bool &ok)
{
    ok = true;
    if((a.type != ITEM_OPERAND) || (b.type != ITEM_OPERAND))
    {
        ok = false;
        return -1;
    }
    return a.data.operand || b.data.operand;
}
