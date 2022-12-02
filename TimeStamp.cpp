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
    return us;
}

quint64 TimeStamp::deltaUs(const quint64 prevTs_us, const quint64 ts_us) const
{
    quint64 delta;
    quint64 maxUs;

    if(ts_us >= prevTs_us)
        delta = ts_us - prevTs_us;
    else
    {
        maxUs = (quint64)std::numeric_limits<quint16>::max()*100*1000; // maximum tenths of second in us
        maxUs += 100000; // + max us
        delta = maxUs - prevTs_us;
        delta += (ts_us + 1);
    }
    return delta;
}


quint64 TimeStamp::computeAccumUs_fromUs(const quint64 ts_us)
{
    switch(m_stage)
    {
    case STAGE_PLAY:
        m_accumUs += deltaUs(m_prevTsUs, ts_us);
        break;

    case STAGE_PAUSE:
        m_stage = STAGE_PLAY;
        break;

    case STAGE_RESET:
        m_stage = STAGE_PLAY;
        m_accumUs = 0;
        break;
    }
    m_prevTsUs = ts_us;
    return m_accumUs;
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
