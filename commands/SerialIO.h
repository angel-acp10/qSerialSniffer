#ifndef SERIALIO_H
#define SERIALIO_H

#include <QSerialPort>
#include <QObject>
#include <QTimer>

class SerialIO : public QObject
{
    Q_OBJECT

public:
    SerialIO(QObject *parent=nullptr);
    ~SerialIO();
    void moveChildrenToThread(QThread *thread);

    bool isBusy();

public slots:
    // serial port control and configuration
    void open();
    void close();
    void setBaudRate(const qint32 baudRate);
    void setDataBits(const QSerialPort::DataBits dataBits);
    void setParity(const QSerialPort::Parity parity);
    void setPortName(const QString &name);
    void setStopBits(const QSerialPort::StopBits stopBits);
    void setTimeout(const unsigned int ms);

    void startRequest(const QByteArray &request);

signals:
    void replyReceived(const QByteArray &response);
    void errorOccurred(QSerialPort::SerialPortError error);

private:
    bool m_busy;
    QTimer *m_timer;
    int m_rxLen;
    QByteArray m_rxBuff;
    QSerialPort *m_serial;

private slots:
    void onReadBytes();
    void onTimeout();
    void onError(QSerialPort::SerialPortError error);

};

#endif // SERIALIO_H
