#ifndef DEINITUART_H
#define DEINITUART_H

#include "Command.h"

class DeInitUart : public Command
{
    Q_OBJECT
public:
    enum Status{
        STATUS_OK=0x55,
        STATUS_FAIL=0xAA
    };

    explicit DeInitUart(CommandManager *cmd, QObject *parent = nullptr);
    ~DeInitUart();

    void read(const QByteArray &in) override;

public slots:
    void write() const;

signals:
    void received(const DeInitUart::Status status);
};

#endif // DEINITUART_H
