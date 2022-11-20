#include "GetId.h"
#include "CommandManager.h"

GetId::GetId(CommandManager *cmd, QObject *parent)
    : Command{cmd, parent}
{
}

GetId::~GetId()
{
}

void GetId::write() const
{
    uint16_t cmdLen = 4;
    QByteArray out;

    out.resize(cmdLen);

    // cmd start
    m_cmd->setHeader(out, 0x01);
    m_cmd->setChecksum(out);
    // cmd end

    m_cmd->addRequest(out, (Command *)this);
}

void GetId::read(const QByteArray &in)
{
    Status status = (Status)in[3];
    QString id = QString(in.mid(4, 25));
    emit received(status, id);
}
