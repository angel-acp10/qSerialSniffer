#ifndef SERIALIO_H
#define SERIALIO_H

#include <QSerialPort>
#include <QTimer>
#include <QQueue>
#include "Command.h"


class SerialIO : public QSerialPort
{
    Q_OBJECT

public:
    explicit SerialIO(QObject *parent = nullptr);
    ~SerialIO();

    typedef struct{
        QByteArray txCmd;
        Command *cmdPtr;
    }request_t;

    void setTimeout(const int timeout);
    int getTimeout() const;

    void setAttempts(const int attempts);
    int getAttempts() const;

public slots:
    void addRequest(QByteArray &txCmd, Command *cmdPtr);

private slots:
    void dequeueRequest();
    void writeRequest();
    void onWriteTimeout();
    void onWrittenBytes(const qint64 bytes);
    void onReadTimeout();
    void onReadBytes();

    void onError();

private:
    typedef enum{
        STAGE_IDLE=0,
        STAGE_WRITE,
        STAGE_READ
    }stage_t;

    int m_maxTimeout;
    int m_maxAttempts;

    int m_remainingAttempts;
    stage_t m_stage;
    qint64 m_txLen;
    qint64 m_rxLen;
    QByteArray m_rxBuff;

    QTimer * m_writeTimer;
    QTimer * m_readTimer;

    request_t m_currRequest;
    QQueue<request_t> m_requests;

signals:
    void writeTimeout();
    void readTimeout();
    // custom error, qSerialPort already has an error signal

};


#endif // SERIALTHREAD_H
