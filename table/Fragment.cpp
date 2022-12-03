#include "Fragment.h"

Fragment::Fragment()
{

}

Fragment::~Fragment()
{

}

Fragment::Port Fragment::getPort() const
{
    return m_port;
}
void Fragment::setPort(const Fragment::Port port)
{
    m_port = port;
}

quint64 Fragment::getStartAcumUs() const
{
    return m_startAcumUs;
}
void Fragment::setStartAcumUs(const quint64 startAcumUs)
{
    m_startAcumUs = startAcumUs;
}

quint64 Fragment::getEndAcumUs() const
{
    return m_endAcumUs;
}
void Fragment::setEndAcumUs(const quint64 endAcumUs)
{
    m_endAcumUs = endAcumUs;
}

QByteArray Fragment::getData() const
{
    return m_data;
}
void Fragment::setData(const QByteArray& data)
{
    m_data = data;
}
