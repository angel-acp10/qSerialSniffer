#include "DeInitUart.h"
#include "CommandManager.h"

DeInitUart::DeInitUart(CommandManager *cmd, QObject *parent)
    : Command{cmd, parent}
{
}

DeInitUart::~DeInitUart()
{
}

void DeInitUart::write() const
{
    uint16_t cmdLen = 4;
    QByteArray out;

    out.resize(cmdLen);

    // cmd start
    m_cmd->setHeader(out, 0x03); // bytes 0, 1, 2
    m_cmd->setChecksum(out); // byte 9
    // cmd end

    m_cmd->addRequest(out, (Command *)this);
}

void DeInitUart::read(const QByteArray &in)
{
    Status status = (Status)in[3];
    emit received(status);
}
