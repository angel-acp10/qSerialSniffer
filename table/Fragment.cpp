#include "Fragment.h"

Fragment::Fragment()
{

}

Fragment::~Fragment()
{

}

quint64 Fragment::getNumber() const
{
    return m_num;
}
void Fragment::setNumber(const quint64 number)
{
    m_num = number;
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

const QByteArray& Fragment::getData() const
{
    return m_data;
}
void Fragment::setData(const QByteArray& data)
{
    m_data = data;
}

const QString& Fragment::getAsciiHex() const
{
    return m_encAsciiHex;
}
void Fragment::setAsciiHex(const QString& asciiHex)
{
    m_encAsciiHex = asciiHex;
}

const QString& Fragment::getHex() const
{
    return m_encHex;
}
void Fragment::setHex(const QString& hex)
{
    m_encHex = hex;
}

const QString& Fragment::getDec() const
{
    return m_encDec;
}
void Fragment::setDec(const QString& dec)
{
    m_encDec = dec; 
}

const QString& Fragment::getBin() const
{
    return m_encBin;
}
void Fragment::setBin(const QString& bin)
{
    m_encBin = bin;
}
