#ifndef TREENODE_H
#define TREENODE_H

#include <memory>
#include <string>
#include <unordered_map>

//------------------------------------------
//             AbstractNode
//------------------------------------------
class AbstractNode
{
public:
    virtual ~AbstractNode(){};
    virtual std::string toString() const = 0;
    virtual int eval(bool &ok) const = 0;
};

//------------------------------------------
//        AbstractNode subclasses
//------------------------------------------
class TreeRoot : public AbstractNode
{
public:
    TreeRoot(std::unique_ptr<AbstractNode> inner);
    ~TreeRoot();
    std::string toString() const override;
    int eval(bool &ok) const override;

    const std::unique_ptr<AbstractNode> m_inner;
};

class NumberNode : public AbstractNode
{
public:
    NumberNode(const int val);
    ~NumberNode();
    std::string toString() const override;
    int eval(bool &ok) const override;

    const int m_val;
};

class VariableNode : public AbstractNode
{
public:
    struct VariableInfo {
        void *ptr;
        enum{
            kUint8=0,
            kUint16,
            kUint32,
            kUint64
        }varType;
    };
    VariableNode(const std::string &name,
                 const std::unordered_map<std::string,VariableInfo> *varsMap);
    ~VariableNode();
    std::string toString() const override;
    int eval(bool &ok) const override;

    const std::string m_name;
    const std::unordered_map<std::string,VariableInfo> *m_varsMap;
};

class ParenthesesNode : public AbstractNode
{
public:
    ParenthesesNode(std::unique_ptr<AbstractNode> inner);
    ~ParenthesesNode();

    std::string toString() const override;
    int eval(bool &ok) const override;

    const std::unique_ptr<AbstractNode> m_inner;
};

class ArrayNode : public AbstractNode
{
public:
    struct ArrayInfo {
        void *ptr;
        int maxL;
        enum{
            kUint8=0,
            kUint16,
            kUint32,
            kUint64
        }arrType;
    };

    ArrayNode(const std::string &name,
              const std::unordered_map<std::string,ArrayInfo> *arrsMap,
              std::unique_ptr<AbstractNode> inner);
    ~ArrayNode();

    std::string toString() const override;
    int eval(bool &ok) const override;

    const std::string m_name;
    const std::unordered_map<std::string,ArrayInfo> *m_arrsMap;
    const std::unique_ptr<AbstractNode> m_inner;
};

class UOperatorNode : public AbstractNode
{
public:
    UOperatorNode(const std::string op,
                  int(*const operation)(int, bool&),
                  std::unique_ptr<AbstractNode> inner);
    ~UOperatorNode();
    std::string toString() const override;
    int eval(bool &ok) const override;

    const std::string m_op;
    int(*const m_operation)(int inner, bool &ok);
    const std::unique_ptr<AbstractNode> m_inner;
};

class BOperatorNode : public AbstractNode
{
public:
    BOperatorNode(const std::string op,
                  int(*const operation)(int, int, bool&),
                  std::unique_ptr<AbstractNode> left,
                  std::unique_ptr<AbstractNode> right);
    ~BOperatorNode();
    std::string toString() const override;
    int eval(bool &ok) const override;

    const std::string m_op;
    int(*const m_operation)(int l, int r, bool &ok);
    const std::unique_ptr<AbstractNode> m_left;
    const std::unique_ptr<AbstractNode> m_right;
};

#endif // TREENODE_H
