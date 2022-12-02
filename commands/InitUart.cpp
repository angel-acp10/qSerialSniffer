#include "InitUart.h"
#include "CommandManager.h"

InitUart::InitUart(CommandManager *cmd, QObject *parent)
    : Command{cmd, parent}
{
}

InitUart::~InitUart()
{
}

void InitUart::write(const uint32_t baudrate,
                     const DataSize dSize,
                     const Parity parity,
                     const Stop stop) const
{
    uint16_t cmdLen = 10;
    QByteArray out;

    out.resize(cmdLen);

    // cmd start
    m_cmd->setHeader(out, 0x02); // bytes 0, 1, 2
    out[3] = (char)0x00; // mode - unused
    out[4] = (char)((baudrate&0x00FF0000)>>16);
    out[5] = (char)((baudrate&0x0000FF00)>>8);
    out[6] = (char) (baudrate&0x000000FF);
    out[7] = (char)dSize;
    out[7] = out[7] | (char)parity;
    out[7] = out[7] | (char)stop;
    out[8] = (char)0x00; // clock - unused
    m_cmd->setChecksum(out); // byte 9
    // cmd end

    m_cmd->addRequest(out, (Command *)this);
}

void InitUart::read(const QByteArray &in)
{
    Status status = (Status)in[3];
    QString id = QString(in.mid(4, 25));
    emit received(status, id);
}
