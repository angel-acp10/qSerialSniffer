#include "Command.h"

Command::Command(SerialIO *ser, QObject *parent)
    : QObject{parent},
      m_ser(ser)
{
}

Command::~Command()
{
}

void Command::read(const QByteArray &in)
{
    (void)in; // unused
}


void Command::setHeader(QByteArray &buff, const uint8_t cmdIdx) const
{
    uint16_t numBytes = buff.length() - 3;

    buff[0] = (char)cmdIdx;
    buff[1] = (char)((numBytes & 0xFF00)>>8);
    buff[2] = (char)(numBytes & 0x00FF);
}

void Command::setChecksum(QByteArray &buff) const
{
    uint8_t res = 0;
    int len = buff.length();

    for(int i=0; i<len-1; i++)
        res += buff[i];

    buff[len-1] = (char)~res;
}

bool Command::verifyChecksum(const QByteArray &buff) const
{
    uint8_t res = 0;
    int len = buff.length();

    for(int i=0; i<len-1; i++)
        res += buff[i];

    if((char)~res == buff[len-1])
        return true;
    return false;
}
