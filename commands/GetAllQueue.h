#ifndef GETALLQUEUE_H
#define GETALLQUEUE_H

#include "Command.h"
#include "TimeStamp.h"

class GetAllQueue : public Command
{
    Q_OBJECT
public:
    enum Status{
        STATUS_OK=0x55,
        STATUS_EMPTY_FRAGQUEUE=0x56,
        STATUS_TXCIRC1_OVF=0xAB,
        STATUS_RXCIRC2_OVF=0xAC,
        STATUS_RXCIRC3_OVF=0xAD
    };

    explicit GetAllQueue(CommandManager *cmd, TimeStamp *tStamp, QObject *parent = nullptr);
    ~GetAllQueue();

    void read(const QByteArray &in) override;

public slots:
    void write() const;

signals:
    void overflow(const GetAllQueue::Status);
    void emptyFragmentsQueue(const quint64 us);
    void received();

private:
    TimeStamp *m_tStamp;
};

#endif // GETALLQUEUE_H
