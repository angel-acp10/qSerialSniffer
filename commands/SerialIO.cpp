#include "SerialIO.h"
#include <QTimer>


SerialIO::SerialIO(QObject *parent) :
    QSerialPort(parent),
    m_maxTimeout(50),
    m_maxAttempts(3),
    m_remainingAttempts(3),
    m_stage(STAGE_IDLE),
    m_txLen(0),
    m_rxLen(0),
    m_rxBuff(4000, 0),
    m_writeTimer(new QTimer(parent)),
    m_readTimer(new QTimer(parent)),
    m_currRequest(),
    m_requests()
{
    m_writeTimer->setSingleShot(true);
    m_readTimer->setSingleShot(true);
    connect(m_writeTimer, &QTimer::timeout, this, &SerialIO::onWriteTimeout);
    connect(m_readTimer, &QTimer::timeout, this, &SerialIO::onReadTimeout);

    connect(this, &QSerialPort::bytesWritten, this, &SerialIO::onWrittenBytes);
    connect(this, &QSerialPort::errorOccurred, this, &SerialIO::onError);
}

SerialIO::~SerialIO()
{
    if(m_requests.isEmpty() == false)
        m_requests.clear();

}

void SerialIO::addRequest(QByteArray &txCmd, Command *cmdPtr)
{
    request_t request = {
        .txCmd = txCmd,
        .cmdPtr = cmdPtr
    };
    m_requests.enqueue(request);

    if(isOpen() && m_stage==STAGE_IDLE)
    {
        dequeueRequest();
    }
}

void SerialIO::dequeueRequest()
{
    if(m_requests.length() > 0)
    {
        m_currRequest = m_requests.dequeue();
        m_remainingAttempts = m_maxAttempts;
        writeRequest();
    }
}

void SerialIO::writeRequest()
{
    m_stage = STAGE_WRITE;

    m_txLen = 0;
    m_remainingAttempts--;
    write(m_currRequest.txCmd);

    m_writeTimer->start(m_maxTimeout);
}

void SerialIO::onWriteTimeout()
{
    if(m_remainingAttempts > 0)
        writeRequest();
    else
    {
        m_stage = STAGE_IDLE;
        dequeueRequest();
    }
}

void SerialIO::onWrittenBytes(const qint64 bytes)
{
    m_writeTimer->start(m_maxTimeout);

    m_txLen += bytes;
    if(bytes >= m_currRequest.txCmd.length())
    {
        // write completed
        m_writeTimer->stop();
        m_rxLen = 0;
        m_stage = STAGE_READ;
        connect(this, &QSerialPort::readyRead, this, &SerialIO::onReadBytes);
        m_readTimer->start(m_maxTimeout);
    }
}

void SerialIO::onReadTimeout()
{
    disconnect(this, &QSerialPort::readyRead, this, &SerialIO::onReadBytes);

    if(m_remainingAttempts > 0)
        writeRequest();
    else
    {
        m_stage = STAGE_IDLE;
        dequeueRequest();
    }
}

void SerialIO::onReadBytes()
{
    m_readTimer->start(m_maxTimeout);

    qint64 toRead = bytesAvailable();
    while(toRead != 0)
    {
        // does the response fit?
        if((m_rxLen + toRead) > m_rxBuff.length())
            m_rxBuff.resize(m_rxLen + toRead);

        // copies response to rxBuff
        read( &m_rxBuff[m_rxLen], toRead );
        m_rxLen += toRead;
        toRead = bytesAvailable();
    }

    // check if the received response is completed
    // 2nd and 3rd byte indicates the remaining bytes of the response
    if ( m_rxLen >= 3)
    {
        uint16_t cmdLength = (((uint16_t)m_rxBuff[1]<<8)|(m_rxBuff[2])) + 3;
        if ( m_rxLen >= cmdLength )
        {
            disconnect(this, &QSerialPort::readyRead, this, &SerialIO::onReadBytes);
            m_readTimer->stop();
            m_currRequest.cmdPtr->read(m_rxBuff.mid(0, cmdLength));
            m_stage = STAGE_IDLE;
            dequeueRequest();
        }
    }
}

void SerialIO::onError()
{
    disconnect(this, &QSerialPort::readyRead, this, &SerialIO::onReadBytes);
    m_readTimer->stop();
    m_writeTimer->stop();
    m_stage = STAGE_IDLE;
    m_requests.clear();
}
