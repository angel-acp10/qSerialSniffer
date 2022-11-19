#ifndef SERIALIO_H
#define SERIALIO_H

#include <QSerialPort>
#include <QThread>
#include <QQueue>
#include "Command.h"


class SerialIO : public QThread
{
    Q_OBJECT

public:
    explicit SerialIO(QObject *parent = nullptr);
    ~SerialIO();

    bool open(const QString &portName, const int baudrate);
    bool close();

    void setTimeout(const int timeout);
    int getTimeout() const;

    void setAttempts(const int attempts);
    int getAttempts() const;

    void addRequest(QByteArray &txCmd, Command *cmdPtr);

signals:
    void error(const QString &s);
    void timeout(const QString &s);

private:
    typedef enum{
        STAGE_CLOSED=0,
        STAGE_KILL,
        STAGE_DEQUEUE,
        STAGE_WRITE,
        STAGE_READ
    }stage_t;

    typedef struct{
        QByteArray txCmd;
        Command *cmdPtr;
    }request_t;

    void run() override;


    QQueue<request_t> m_requests;
    QSerialPort *m_serial;
    stage_t m_stage;

    int m_timeout;
    int m_attempts;
};


#endif // SERIALTHREAD_H
