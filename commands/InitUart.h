#ifndef INITUART_H
#define INITUART_H

#include "Command.h"

class InitUart : public Command
{
    Q_OBJECT
public:
    enum DataSize{
        DATASIZE_8bits = (0<<7),
        DATASIZE_9bits = (1<<7)
    };

    enum Parity {
        PARITY_NONE = (0b00<<5),
        PARITY_EVEN = (0b01<<5),
        PARITY_ODD  = (0b10<<5)
    };

    enum Stop{
        STOP_1bit = (0b00<<3),
        STOP_2bits = (0b01<<3)
    };

    enum Status{
        STATUS_OK=0x55,
        STATUS_FAIL=0xAA
    };

    explicit InitUart(CommandManager *cmd, QObject *parent = nullptr);
    ~InitUart();

    void read(const QByteArray &in) override;

public slots:
    void write(const uint32_t baudrate,
                const InitUart::DataSize dSize,
                const InitUart::Parity parity,
                const InitUart::Stop stop) const;

signals:
    void received(const InitUart::Status status, const QString &id);
};

#endif // INITUART_H
