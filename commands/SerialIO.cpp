#include "SerialIO.h"
#include <QTime>


SerialIO::SerialIO(QObject *parent) :
    QThread(parent),
    m_requests(),
    m_serial(new QSerialPort),
    m_stage(STAGE_CLOSED),
    m_timeout(50),
    m_attempts(3)
{
}

SerialIO::~SerialIO()
{
    if(m_requests.isEmpty() == false)
        m_requests.clear();
    m_stage = STAGE_KILL;
    wait();
    delete m_serial;
}

bool SerialIO::open(const QString &portName, const int baudrate)
{
    if(m_serial->isOpen())
    {
        // if it is already open, check if port or baudrate have changed
        if( (m_serial->portName() == portName) && (m_serial->baudRate() == baudrate) )
            return true; // nothing to do

        close();
    }
    m_requests.clear();

    // open
    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudrate);
    m_serial->setDataBits( QSerialPort::Data8 );
    m_serial->setParity( QSerialPort::NoParity );
    m_serial->setStopBits( QSerialPort::OneStop );
    if(m_serial->open(QIODeviceBase::ReadWrite) == false)
    {
        emit error(tr("Can't open %1, error code %2").arg(portName).arg(m_serial->error()));
        return false;
    }
    m_stage = STAGE_DEQUEUE;
    return true;
}

bool SerialIO::close()
{
    m_stage = STAGE_CLOSED;
    while(m_serial->isOpen())
        this->msleep(10);

    return true;
}

void SerialIO::addRequest(QByteArray &txCmd, Command *cmdPtr)
{
    if (txCmd.isEmpty() == true || cmdPtr==nullptr)
    {
        emit error("Nothing to send.");
        return;
    }

    request_t request = {
        .txCmd = txCmd,
        .cmdPtr = cmdPtr
    };
    m_requests.enqueue(request);

    if (!isRunning())
        start();
}

void SerialIO::run()
{
    request_t request;
    QByteArray rxBuff;
    int attempts = 0;
    int rxLen = 0;
    rxBuff.resize(4000);

    while(m_stage != STAGE_KILL)
    {
        switch(m_stage)
        {
        case STAGE_CLOSED:
        case STAGE_KILL:
            m_serial->close();
            while(m_stage==STAGE_CLOSED)
                this->msleep(10);
            break;

        case STAGE_DEQUEUE:
            if(m_requests.isEmpty())
            {
                this->msleep(10);
                continue;
            }
            request = m_requests.dequeue();
            attempts = m_attempts;
            m_stage = STAGE_WRITE;
            break;

        case STAGE_WRITE:
            m_serial->clear();  // elimina cualquier caracter que hubiera tanto rx como tx
            if (m_serial->write(request.txCmd) == -1) // check error
            {
                m_stage = STAGE_DEQUEUE;
                emit error(tr("Failed to write on COM port. An error occurred"));
                break;
            }
            if (m_serial->waitForBytesWritten(m_timeout) == false)
            {
                m_stage = STAGE_DEQUEUE;
                emit this->timeout("Write timeout.");
                break;
            }
            m_stage = STAGE_READ;
            rxLen = 0;
            break;

        case STAGE_READ:
            if (m_serial->waitForReadyRead(m_timeout) == false)
            {
                // device did not reply
                attempts--;
                if(attempts == 0)
                {
                    m_stage = STAGE_DEQUEUE;
                    emit this->timeout("Read timeout.");
                    break;
                }
            }

            int toRead = m_serial->bytesAvailable();
            while ( toRead != 0 )
            {
                // does the response fit?
                if((rxLen + toRead) > rxBuff.length())
                    rxBuff.resize(rxLen + toRead);

                // copies response to rxBuff
                if( m_serial->read( &rxBuff[rxLen], toRead ) == -1 )
                {
                    m_stage = STAGE_CLOSED;
                    emit this->timeout("Port closed, unable to read");
                }
                rxLen += toRead;
                toRead = m_serial->bytesAvailable();
            }

            // check if the received response is completed
            // 2nd and 3rd byte indicates the remaining bytes of the response
            if ( rxLen >= 3)
            {
                uint16_t cmdLength = (((uint16_t)rxBuff[1]<<8)|(rxBuff[2])) + 3;
                if ( rxLen >= cmdLength )
                {
                    m_stage = STAGE_DEQUEUE;
                    request.cmdPtr->read(rxBuff.mid(0, cmdLength));
                }
            }
            break;
        }
    }
}
