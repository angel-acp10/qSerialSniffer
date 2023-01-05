#include "SerialIO.h"
#include <QDebug>

SerialIO::SerialIO(QObject *parent)
    : QObject(parent),
      m_busy(false)
{
    m_timer = new QTimer();
    m_timer->setSingleShot(true);
    m_timer->setInterval(300);
    connect(m_timer, &QTimer::timeout, this, &SerialIO::onTimeout);

    m_serial = new QSerialPort();

    m_thread.reset(new QThread); // no parent
    m_serial->moveToThread(m_thread.get());
    m_thread->start();

    connect(m_serial, &QSerialPort::errorOccurred,
            this, &SerialIO::onError);
}

SerialIO::~SerialIO()
{
    if(m_serial->isOpen())
        QMetaObject::invokeMethod(m_serial, "close");
    m_thread->quit();
    m_thread->wait();
    delete m_serial;
    delete m_timer;
}

bool SerialIO::isBusy()
{
    return m_busy;
}

void SerialIO::open()
{
    m_serial->open(QIODeviceBase::ReadWrite);
}
void SerialIO::close()
{
    m_serial->close();
}
void SerialIO::setBaudRate(const qint32 baudRate)
{
    m_serial->setBaudRate(baudRate);
}
void SerialIO::setDataBits(const QSerialPort::DataBits dataBits)
{
    m_serial->setDataBits(dataBits);
}
void SerialIO::setParity(const QSerialPort::Parity parity)
{
    m_serial->setParity(parity);
}
void SerialIO::setPortName(const QString &name)
{
    m_serial->setPortName(name);
}
void SerialIO::setStopBits(const QSerialPort::StopBits stopBits)
{
    m_serial->setStopBits(stopBits);
}
void SerialIO::setTimeout(const unsigned int ms)
{
    m_timer->setInterval(ms);
}

void SerialIO::startRequest(const QByteArray &request)
{
    if(m_busy)
        return;
    m_busy = true;
    m_rxLen = 0;
    m_serial->write(request);
    qDebug() << "SerialIO w:" << request;
    m_timer->start();
    connect(m_serial, &QSerialPort::readyRead,
            this, &SerialIO::onReadBytes);
}

void SerialIO::onReadBytes()
{
    m_timer->start();
    qint64 toRead = m_serial->bytesAvailable();
    while(toRead != 0)
    {
        // does the response fit?
        if((m_rxLen + toRead) > m_rxBuff.length())
            m_rxBuff.resize(m_rxLen + toRead);

        // copies response to rxBuff
        m_serial->read( &m_rxBuff[m_rxLen], toRead );
        m_rxLen += toRead;
        toRead = m_serial->bytesAvailable();
    }

    // check if the received response is completed
    // 2nd and 3rd byte indicates the remaining bytes of the response
    if ( m_rxLen >= 3)
    {
        uint16_t cmdLength;
        cmdLength = (m_rxBuff[1]<<8)&0xFF00;
        cmdLength = cmdLength | (m_rxBuff[2]&0x00FF);
        cmdLength += 3;
        if ( m_rxLen >= cmdLength )
        {
            m_timer->stop();
            disconnect(m_serial, &QSerialPort::readyRead, this, nullptr);
            emit replyReceived(m_rxBuff.mid(0, cmdLength));
            qDebug() << "SerialIO r:" << m_rxBuff.mid(0, cmdLength);
            m_busy = false;
        }
    }
}

void SerialIO::onTimeout()
{
    disconnect(m_serial, &QSerialPort::readyRead, this, nullptr);
    emit errorOccurred(QSerialPort::SerialPortError::TimeoutError);
    m_busy = false;
}

void SerialIO::onError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::SerialPortError::NoError)
        return;

    disconnect(m_serial, &QSerialPort::readyRead, this, nullptr);
    emit errorOccurred(error);
    m_busy = false;
    qCritical() << "SerialIO ERROR:" << error;
}
