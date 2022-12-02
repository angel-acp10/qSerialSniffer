#include "GetAllQueue.h"
#include "CommandManager.h"

#include <limits>

GetAllQueue::GetAllQueue(CommandManager *cmd, TimeStamp *tStamp0, TimeStamp *tStamp1, QObject *parent)
    : Command{cmd, parent},
      m_tStamp0(tStamp0),
      m_tStamp1(tStamp1)
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
        ts_raw = (quint32)(in[4]&0x000000FF);
        ts_raw <<= 8;
        ts_raw |= (quint32)(in[5]&0x000000FF);
        ts_raw <<= 8;
        ts_raw |= (quint32)(in[6]&0x000000FF);
        ts_raw <<= 8;
        ts_raw |= (quint32)(in[7]&0x000000FF);
        emit emptyFragmentsQueue( m_tStamp0->computeAccumUs_fromRaw(ts_raw) );
        m_tStamp1->computeAccumUs_fromRaw(ts_raw);
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
    TimeStamp * tStamp;


    // port
    outFrag.setPort((Fragment::Port)in[st+0]);

    switch(outFrag.getPort())
    {
    case 2: tStamp = m_tStamp0; break;
    case 3: tStamp = m_tStamp1; break;
    }

    // number of bytes until the end of the fragment
    nBytes  = ((quint16)in[st+1]<<8);
    nBytes |= ((quint16)in[st+2]);

    // numBytes - starts counting from here (0)

    start_raw = (quint32)(in[st+3]&0x000000FF);
    start_raw <<= 8;
    start_raw |= (quint32)(in[st+4]&0x000000FF);
    start_raw <<= 8;
    start_raw |= (quint32)(in[st+5]&0x000000FF);
    start_raw <<= 8;
    start_raw |= (quint32)(in[st+6]&0x000000FF);
    outFrag.setStartAcumUs( tStamp->computeAccumUs_fromRaw(start_raw) );

    end_raw = (quint32)(in[st+7]&0x000000FF);
    end_raw <<= 8;
    end_raw |= (quint32)(in[st+8]&0x000000FF);
    end_raw <<= 8;
    end_raw |= (quint32)(in[st+9]&0x000000FF);
    end_raw <<= 8;
    end_raw |= (quint32)(in[st+10]&0x000000FF);
    outFrag.setEndAcumUs( tStamp->computeAccumUs_fromRaw(end_raw) );
    // remaining bytes until end of the fragment are data
    outFrag.setData( in.mid(st+11, nBytes-8) ); // 8 bits are not data (are start_raw and end_raw)

    // end of fragment

    quint16 nextFragStart = st + 3 + nBytes;
    if(nextFragStart >= in.length()-1)
        return 0;
    return nextFragStart;
}

