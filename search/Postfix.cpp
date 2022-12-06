#include "Postfix.h"

Postfix::Postfix() :
    m_infix(""),
    m_postfix(""),
    m_stack(),

    m_operatorPrior( {
                        {"*", "/", "%"}, // 0 - top priority
                        {"+", "-"},
                        {"<<", ">>"},
                        {"<", "<=", ">", ">="},
                        {"==", "!="},
                        {"&"},
                        {"^"},
                        {"|"},
                        {"&&"},
                        {"||"} // 9 - lowest priority
                    } ),
    m_operatorLst2(),
    m_operatorLst1()
{
    for(auto &group : m_operatorPrior)
    {
        for(auto &op : group)
        {
            switch(op.length())
            {
            case 1:  m_operatorLst1.push_back(op); break;
            case 2:  m_operatorLst2.push_back(op); break;
            default: break;
            }
        }
    }
}

Postfix::~Postfix()
{
}

void Postfix::setInfix(std::string &infix)
{
    m_infix = infix;
}

std::string Postfix::getInfix() const
{
    return m_infix;
}

bool Postfix::convert()
{
    std::string in = m_infix;
    std::string op;

    m_postfix.clear();
    while(in.length() > 0)
    {
        op = startsWithOperator(in);
        if(op != "") // starts with operator == true
        {
            int in_optorPrior = getOperatorPiority(op);

            while(!m_stack.empty() &&
                  getOperatorPiority(m_stack.top()) >= in_optorPrior)
            {
                m_postfix.append(m_stack.top()+' ');
                m_stack.pop();
            }
            m_stack.push(op);
            in.erase(0,op.length());
        }
        else if(in[0] == '(')
        {
            m_stack.push("(");
            in.erase(0,1);
        }
        else if(in[0] == ')')
        {
            while(m_stack.top() != "(")
            {
                m_postfix.append(m_stack.top()+' ');
                m_stack.pop();
            }
            m_stack.pop(); // discard "("
            in.erase(0,1);
        }
        else
        {
            // it can only be an operand
            std::string operand = getOperand(in);
            m_postfix.append(operand+' ');
            in.erase(0, operand.length());
        }
    }
    while(!m_stack.empty())
    {
        m_postfix.append(m_stack.top()+' ');
        m_stack.pop();
    }

    return true;
}

std::string Postfix::getPostfix() const
{
    return m_postfix;
}

std::string Postfix::startsWithOperator(std::string &infix) const
{
    for(auto &op : m_operatorLst2)
    {
        if(infix.rfind(op, 0) == 0)
            return op;
    }
    for(auto &op : m_operatorLst1)
    {
        if(infix.rfind(op, 0) == 0)
            return op;
    }
    return "";
}

int Postfix::getOperatorPiority(std::string &operatr) const
{
    for(int prior=0; prior<(int)m_operatorPrior.size(); prior++)
    {
        for(auto &op : m_operatorPrior[prior])
        {
            if(op == operatr)
                return prior;
        }
    }
    return -1; // operator does not exist
}

std::string Postfix::getOperand(std::string &infix) const
{
    int len = 0;
    for(int i=0; i<(int)infix.length(); i++)
    {
        if( (infix[i]>='0' && infix[i]<='9') ||
            (infix[i]>='A' && infix[i]<='Z') ||
            (infix[i]>='a' && infix[i]<='z') ||
             infix[i]=='_' )
        {
            len++;
        }
        else
            break;
    }
    if(len!=0)
        return infix.substr(0, len);
    return "";
}
