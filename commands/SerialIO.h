#ifndef SERIALIO_H
#define SERIALIO_H

#include <QSerialPort>
#include <QTimer>

class SerialIO : public QSerialPort
{
    Q_OBJECT

public:
    explicit SerialIO(QObject *parent = nullptr);
    ~SerialIO();

    typedef enum{
        STAGE_IDLE=0,
        STAGE_WRITE,
        STAGE_READ
    }stage_t;

    stage_t getStage() const;

    void setTimeout(const int timeout);
    int getTimeout() const;

    void setAttempts(const int attempts);
    int getAttempts() const;

public slots:
    void startRequest(QByteArray &txBuff);

private slots:
    void writeRequest();
    void onWriteTimeout();
    void onWrittenBytes(const qint64 bytes);
    void onReadTimeout();
    void onReadBytes();

    void onError();

private:
    int m_maxTimeout;
    int m_maxAttempts;

    int m_remainingAttempts;
    stage_t m_stage;
    qint64 m_txLen;
    qint64 m_rxLen;
    QByteArray m_txBuff;
    QByteArray m_rxBuff;

    QTimer m_writeTimer;
    QTimer m_readTimer;

signals:
    void maxAttempts();
    void replyReceived(const QByteArray reply);
    // custom error, qSerialPort already has an error signal

};


#endif // SERIALIO_H
