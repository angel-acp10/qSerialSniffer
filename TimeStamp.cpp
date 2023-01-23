#include "TimeStamp.h"

#include <QDateTime>

TimeStamp::TimeStamp(QObject *parent) :
    QObject{parent},
    m_stage(STAGE_RESET),
    m_accumUs(0),
    m_prevTsUs(0),
    m_onPauseTsUnix(0)
{

}

TimeStamp::~TimeStamp()
{

}

quint64 TimeStamp::rawToUs(const quint32 ts_raw) const
{
    quint16 tenths = (ts_raw & 0xFFFF0000)>>16; //tenths of second
    quint64 us = (ts_raw & 0x0000FFFF)*2; // lower 16 bits are a counter of 2us

    us += ((quint64)tenths*100*1000);
    qDebug() << us;
    return us;
}

qint64 TimeStamp::deltaUs(const quint64 prevTs_us, const quint64 ts_us) const
{
    qint64 delta;
    static const quint64 maxUs =
            (quint64)std::numeric_limits<quint16>::max()*100*1000 + // tenths of seconds converted to us
            100000; // max us

    delta = ts_us - prevTs_us;

    if(delta >= 0) // ts_us is the most recent
        return delta; // positive

    else if(static_cast<quint64>(-delta) < maxUs/2) // ts_us is not the most recent, but is still not considered overflow
        return delta; // negative

    else //  ts_us is the most recent, but there's an overflow
    {
        //qDebug() << "OVERFLOW!!! on prevTsUs:" << m_prevTsUs << " TsUs:" << ts_us;
        delta = maxUs - prevTs_us;
        delta += (ts_us + 1);
        return delta; // positive
    }
}


quint64 TimeStamp::computeAccumUs_fromUs(const quint64 ts_us)
{
    qint64 delta;
    quint64 res;
    switch(m_stage)
    {
    case STAGE_PLAY:
        delta = deltaUs(m_prevTsUs, ts_us);
        res = m_accumUs + delta;
        if(delta > 0)
        {
            m_prevTsUs = ts_us;
            m_accumUs = res; // only update m_accumUs if it's the newest point
        }
        break;

    case STAGE_PAUSE:
        m_stage = STAGE_PLAY;
        res = m_accumUs;
        break;

    case STAGE_RESET:
        m_stage = STAGE_PLAY;
        res = 0;
        m_accumUs = 0;
        break;
    }
    //qDebug() << "prevTsUs:" << m_prevTsUs << " TsUs:" << ts_us << "AccumUs:" << m_accumUs;
    //m_prevTsUs = ts_us;
    return res;
}

quint64 TimeStamp::computeAccumUs_fromRaw(const quint32 ts_raw)
{
    quint64 ts_us = rawToUs(ts_raw);
    return computeAccumUs_fromUs(ts_us);
}

void TimeStamp::resetAccumUs()
{
    m_stage = STAGE_RESET;
}

void TimeStamp::onPauseClicked()
{
    m_stage = STAGE_PAUSE;
    m_onPauseTsUnix = QDateTime::currentMSecsSinceEpoch();
}

void TimeStamp::onPlayClicked()
{
    quint64 onPlayTsUnix = QDateTime::currentMSecsSinceEpoch();
    quint64 delta_ms = onPlayTsUnix - m_onPauseTsUnix;
    m_accumUs += (delta_ms*1000);
}
