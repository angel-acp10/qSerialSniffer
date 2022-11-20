#include "GetAllQueue.h"
#include "CommandManager.h"

#include <limits>

GetAllQueue::GetAllQueue(CommandManager *cmd, TimeStamp *tStamp, QObject *parent)
    : Command{cmd, parent},
      m_tStamp(tStamp)
{
}

GetAllQueue::~GetAllQueue()
{
}

void GetAllQueue::write() const
{
    uint16_t cmdLen = 4;
    QByteArray out;

    out.resize(cmdLen);

    // cmd start
    m_cmd->setHeader(out, 0x04);
    m_cmd->setChecksum(out);
    // cmd end

    m_cmd->addRequest(out, (Command *)this);
}

void GetAllQueue::read(const QByteArray &in)
{
    quint32 start_raw;
    quint32 end_raw;
    quint64 acumUs;

    Status status = (Status)in[3];
    switch(status)
    {
    case STATUS_TXCIRC1_OVF:
    case STATUS_RXCIRC2_OVF:
    case STATUS_RXCIRC3_OVF:
        emit overflow(status);
        return;

    case STATUS_EMPTY_FRAGQUEUE:
        start_raw  = ((quint32)in[4])<<24;
        start_raw |= ((quint32)in[5])<<16;
        start_raw |= ((quint32)in[6])<<8;
        start_raw |= ((quint32)in[7]);
        acumUs = m_tStamp->computeAccumUs_fromRaw(start_raw);
        emit emptyFragmentsQueue(acumUs);
        return;

    case STATUS_OK:
        break;

    default:
        return;
    }

    //emit received(status, id);
}

