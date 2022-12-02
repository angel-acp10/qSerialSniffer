#ifndef SERIALIO_H
#define SERIALIO_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>


class SerialIO : public QThread
{
    Q_OBJECT

public:
    typedef enum
    {
        STAGE_OFF=0,
        STAGE_IDLE,
        STAGE_WRITE,
        STAGE_READ
    }stage_t;

    explicit SerialIO(int timeout, int attempts, QObject *parent = nullptr);
    ~SerialIO();

    stage_t getStage() const;

public slots:
    void startRequest(/*const QString &portName,*/ const QByteArray &request);

signals:
    void replyReceived(const QByteArray &response);
    void error(const QString &s);
    void maxAttempts();

private:


    void run() override;

    stage_t m_stage;
    int m_timeout;
    int m_attempts;
    bool m_quit;
    bool m_newRequest;


    QString m_portName;
    QByteArray m_request;
    QMutex m_mutex;
    QWaitCondition m_cond;
};


#endif // SERIALIO_H
