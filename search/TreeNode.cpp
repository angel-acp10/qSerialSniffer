#include "TreeNode.h"
#include <iostream>

//------------------------------------------
//            TreeRoot
//------------------------------------------
TreeRoot::TreeRoot(std::unique_ptr<AbstractNode> inner)
    : m_inner(std::move(inner))
{
}
TreeRoot::~TreeRoot()
{
}
std::string TreeRoot::toString() const
{
    return m_inner->toString();
}
int TreeRoot::eval(bool &ok) const
{
    int res = m_inner->eval(ok);
    if(!ok)
        return -1; // failed to evaluate inner node
    return res;
}

//------------------------------------------
//              NumberNode
//------------------------------------------
NumberNode::NumberNode(const int val)
    : m_val(val)
{
}
NumberNode::~NumberNode()
{
}
std::string NumberNode::toString() const
{
    std::string str = std::to_string(m_val);
    return str;
}
int NumberNode::eval(bool &ok) const
{
    ok = true;
    return m_val;
}

//------------------------------------------
//              VariableNode
//------------------------------------------
VariableNode::VariableNode(const std::string &name,
                           const std::unordered_map<std::string,VariableInfo> *varsMap)
    : m_name(name),
      m_varsMap(varsMap)
{
}
VariableNode::~VariableNode()
{
}
std::string VariableNode::toString() const
{
    return m_name;
}
int VariableNode::eval(bool &ok) const
{
    auto search = m_varsMap->find(m_name);
    if(search == m_varsMap->end())
    {
        ok = false;
        return -1; // variable does not exist in m_varsMap
    }
    VariableInfo var = search->second;

    ok = true;
    switch(var.varType)
    {
    case VariableInfo::kUint8:  return (int)( *(uint8_t*)var.ptr);
    case VariableInfo::kUint16: return (int)(*(uint16_t*)var.ptr);
    case VariableInfo::kUint32: return (int)(*(uint32_t*)var.ptr);
    case VariableInfo::kUint64: return (int)(*(uint64_t*)var.ptr);
    }

    ok = false;
    return -2; // varType is not defined
}

//------------------------------------------
//           ParenthesesNode
//------------------------------------------
ParenthesesNode::ParenthesesNode(std::unique_ptr<AbstractNode> inner)
    : m_inner(std::move(inner))
{
}
ParenthesesNode::~ParenthesesNode()
{
}
std::string ParenthesesNode::toString() const
{
    std::string str = "(" + m_inner->toString() + ")";
    return str;
}
int ParenthesesNode::eval(bool &ok) const
{
    int res = m_inner->eval(ok);
    if(!ok)
        return -1; // failed to evaluate inner node
    return res;
}

//------------------------------------------
//              ArrayNode
//------------------------------------------
ArrayNode::ArrayNode(const std::string &name,
            const std::unordered_map<std::string,ArrayInfo> *arrsMap,
            std::unique_ptr<AbstractNode> inner)
    : m_name(name),
      m_arrsMap(arrsMap),
      m_inner(std::move(inner))
{
}
ArrayNode::~ArrayNode()
{
}
std::string ArrayNode::toString() const
{
    std::string str = m_name + "[" + m_inner->toString() + "]";
    return str;
}
int ArrayNode::eval(bool &ok) const
{
    auto search = m_arrsMap->find(m_name);
    if(search == m_arrsMap->end())
    {
        ok = false;
        return -1; // variable does not exist in m_arrsMap
    }
    ArrayInfo arr = search->second;

    int index = m_inner->eval(ok);
    if(!ok)
    {
        return -1; // failed to evaluate inner node
    }
    else if((index < 0) || (index >= arr.maxL))
    {
        ok = false;
        return -2; // array index out of range
    }
    ok = true;
    switch(arr.arrType)
    {
    case ArrayInfo::kUint8:  return (int)(  (uint8_t*)arr.ptr)[index];
    case ArrayInfo::kUint16: return (int)( (uint16_t*)arr.ptr)[index];
    case ArrayInfo::kUint32: return (int)( (uint32_t*)arr.ptr)[index];
    case ArrayInfo::kUint64: return (int)( (uint64_t*)arr.ptr)[index];
    }

    ok = false;
    return -3; // arrType is not defined
}

//------------------------------------------
//              UOperatorNode
//------------------------------------------
UOperatorNode::UOperatorNode(const std::string op,
                             int(*const operation)(int, bool&),
                             std::unique_ptr<AbstractNode> inner)
    : m_op(op),
      m_operation(operation),
      m_inner(std::move(inner))
{
}
UOperatorNode::~UOperatorNode()
{
}
std::string UOperatorNode::toString() const
{
    std::string str = m_op + m_inner->toString();
    return str;
}
int UOperatorNode::eval(bool &ok) const
{
    int inner = m_inner->eval(ok);
    if(!ok)
        return -1; // failed to evaluate inner node

    int res = m_operation(inner, ok);
    if(!ok)
        return -2; // failed to evaluate current node

    return res;
}

//------------------------------------------
//              BOperatorNode
//------------------------------------------
BOperatorNode::BOperatorNode(const std::string op,
                            int(*const operation)(int, int, bool&),
                            std::unique_ptr<AbstractNode> left,
                            std::unique_ptr<AbstractNode> right)
    : m_op(op),
      m_operation(operation),
      m_left(std::move(left)),
      m_right(std::move(right))
{
}
BOperatorNode::~BOperatorNode()
{
}
std::string BOperatorNode::toString() const
{
    std::string str = m_left->toString() + m_op + m_right->toString();
    return str;
}
int BOperatorNode::eval(bool &ok) const
{
    int left = m_left->eval(ok);
    if(!ok)
        return -1; // failed to evaluate left node

    int right = m_right->eval(ok);
    if(!ok)
        return -2; // failed to evaluate right node

    int res = m_operation(left, right, ok);
    if(!ok)
        return -3; // failed to evaluate current node

    return res;
}

