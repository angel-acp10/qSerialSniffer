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
    quint32 ts_raw;
    Status status = (Status)in[3];

    switch(status)
    {
    case STATUS_TXCIRC1_OVF:
    case STATUS_RXCIRC2_OVF:
    case STATUS_RXCIRC3_OVF:
        emit overflow(status);
        return;

    case STATUS_EMPTY_FRAGQUEUE:
        ts_raw  = ((quint32)in[4])<<24;
        ts_raw |= ((quint32)in[5])<<16;
        ts_raw |= ((quint32)in[6])<<8;
        ts_raw |= ((quint32)in[7]);
        emit emptyFragmentsQueue( m_tStamp->computeAccumUs_fromRaw(ts_raw) );
        return;

    case STATUS_OK:
        parseAllFragments(in);
        break;

    default:
        return;
    }
}

void GetAllQueue::parseAllFragments(const QByteArray &in)
{
    QList<Fragment> fragList;
    Fragment frag;
    quint16 start = 4;

    do{
        start = getFragment(in, start, frag);
        fragList.append(frag);
    }while(start);

    emit received(fragList);
}


/**
 * @brief GetAllQueue::getFragment
 * @param in
 * @param st start index (of "in" array) where the fragment to be extracted starts
 * @param outFrag
 * @return the start of next fragment. 0 if no more fragments exist
 */

quint16 GetAllQueue::getFragment(const QByteArray &in, const int st, Fragment &outFrag) const
{
    quint16 nBytes;
    quint32 start_raw, end_raw;

    // port
    outFrag.setPort((Fragment::Port)in[st+0]);

    // number of bytes until the end of the fragment
    nBytes  = ((quint16)in[st+1]<<8);
    nBytes |= ((quint16)in[st+2]);

    // numBytes - starts counting from here (0)

    start_raw  = ((quint32)in[st+3])<<24;
    start_raw |= ((quint32)in[st+4])<<16;
    start_raw |= ((quint32)in[st+5])<<8;
    start_raw |= ((quint32)in[st+6]);
    outFrag.setStartAcumUs( m_tStamp->computeAccumUs_fromRaw(start_raw) );
    end_raw  = ((quint32)in[st+7])<<24;
    end_raw |= ((quint32)in[st+8])<<16;
    end_raw |= ((quint32)in[st+9])<<8;
    end_raw |= ((quint32)in[st+10]);
    outFrag.setEndAcumUs( m_tStamp->computeAccumUs_fromRaw(end_raw) );
    // remaining bytes until end of the fragment are data
    outFrag.setData( in.mid(st+11, nBytes-8) ); // 8 bits are not data (are start_raw and end_raw)

    // end of fragment

    quint16 nextFragStart = st + 3 + nBytes;
    if(nextFragStart >= in.length()-1)
        return 0;
    return nextFragStart;
}

