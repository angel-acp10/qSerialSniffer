#ifndef GETALLQUEUE_H
#define GETALLQUEUE_H

#include "Command.h"
#include "TimeStamp.h"
#include "../table/Fragment.h"

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

    explicit GetAllQueue(CommandManager *cmd, TimeStamp *tStamp0, TimeStamp *tStamp1, QObject *parent = nullptr);
    ~GetAllQueue();

    void read(const QByteArray &in) override;

public slots:
    void write() const;

signals:
    void overflow(const GetAllQueue::Status);
    void emptyFragmentsQueue(const quint64 us);
    void received(QList<Fragment> fragList);

private:
    TimeStamp *m_tStamp0;
    TimeStamp *m_tStamp1;

    void parseAllFragments(const QByteArray &in);
    quint16 getFragment(const QByteArray &in, const int start, Fragment &outFrag) const;
};

#endif // GETALLQUEUE_H
