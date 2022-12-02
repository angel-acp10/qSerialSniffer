#include "SerialIO.h"

#include <QSerialPort>
#include <QTime>
#include <QDebug>


SerialIO::SerialIO(int timeout, int attempts, QObject *parent) :
    QThread(parent),
    m_stage(STAGE_OFF),
    m_timeout(timeout),
    m_attempts(attempts),
    m_quit(false),
    m_newRequest(false)
{
}


SerialIO::~SerialIO()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

SerialIO::stage_t SerialIO::getStage() const
{
    return m_stage;
}

void SerialIO::startRequest(/*const QString &portName,*/ const QByteArray &request)
{

    const QMutexLocker locker(&m_mutex);
    //m_portName = portName;
    m_request = request;
    m_newRequest = true;

    if (!isRunning())
        start();
    else
        m_cond.wakeOne();
}


void SerialIO::run()
{
    m_mutex.lock();
    QByteArray currRequest = m_request;
    m_mutex.unlock();
    QByteArray response;
    //stage_t stage = STAGE_IDLE;

    QSerialPort serial;
    serial.setPortName("COM6");
    serial.setBaudRate( 115200 );
    //serial.setDataBits( QSerialPort::Data8 );
    //serial.setParity( QSerialPort::NoParity );
    //serial.setStopBits( QSerialPort::OneStop );
    serial.open(QIODevice::ReadWrite);
    m_mutex.lock();
    m_stage = STAGE_IDLE;
    m_mutex.unlock();
    int id = 0;
    uint16_t cmdLength;
    bool debug = true;

    while (!m_quit)
    {
        switch(m_stage)
        {
        case STAGE_IDLE:
            this->msleep(10);

            m_mutex.lock();
            if(m_newRequest)
            {
                id++;
                if(debug) qDebug()<<"id:"<<id;
                m_newRequest = false;
                currRequest = m_request;
                m_stage = STAGE_WRITE;
            }
            m_mutex.unlock();
            break;

        case STAGE_WRITE:
            serial.clear();  // clear rx and tx buffers
            if (serial.write(currRequest) == -1)
            {
                m_mutex.lock();
                m_stage = STAGE_IDLE;
                m_mutex.unlock();
                //emit error(tr("Failed to write on COM port. An error occurred"));
                if(debug) qDebug()<<"id:"<<id<<" Failed to write on COM port. An error occurred";
                break;
            }
            if (serial.waitForBytesWritten(m_timeout) == false)
            {
                m_mutex.lock();
                m_stage = STAGE_IDLE;
                m_mutex.unlock();
                //emit this->timeout(tr("Write timeout."));
                if(debug) qDebug()<<"id:"<<id<<" Write timeout";
                break;
            }
            m_mutex.lock();
            m_stage = STAGE_READ;
            m_mutex.unlock();
            response.clear();
            //this->msleep(100);
            break;

        case STAGE_READ:
            if (serial.waitForReadyRead(m_timeout) == false)
            {
                m_mutex.lock();
                m_stage = STAGE_IDLE;
                m_mutex.unlock();
                if(debug) qDebug()<<serial.error();
                //emit this->timeout(tr("Read timeout."));
                if(debug) qDebug()<<"id:"<<id<<" Read timeout";
                break;
            }

            response += serial.readAll();

            if ( response.size() >= 3)
            {
                cmdLength = (response[1]<<8)&0xFF00;
                cmdLength = cmdLength | (response[2]&0x00FF);
                cmdLength += 3;
                //cmdLength = (((uint16_t)response[1]<<8)|(response[2])) + 3;
                if(debug) qDebug()<<"id:"<<id<<" >3 SIZE::"<<response.size()<<" cmdLength="<<cmdLength;

                if ( response.size() >= cmdLength ) // check if all command bytes were received
                {
                    if(debug) qDebug()<<"id:"<<id<<" emit replyReceived";
                    if(debug) qDebug()<<response.toHex(':');
                    emit this->replyReceived(response);
                    m_mutex.lock();
                    m_stage = STAGE_IDLE;
                    m_mutex.unlock();
                }
            }
            break;

        default:
            break;
        }
    }
    if(serial.isOpen())
        serial.close();
    this->msleep(100);
}
