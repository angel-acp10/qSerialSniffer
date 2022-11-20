#ifndef INITUART_H
#define INITUART_H

#include "Command.h"

class InitUart : public Command
{
    Q_OBJECT
public:
    typedef enum {
        DATASIZE_8bits = (0<<7),
        DATASIZE_9bits = (1<<7)
    }DataSize;

    typedef enum {
        PARITY_NONE = (0b00<<5),
        PARITY_EVEN = (0b01<<5),
        PARITY_ODD  = (0b10<<5)
    }Parity;

    typedef enum {
        STOP_1bit = (0b00<<3),
        STOP_2bits = (0b01<<3)
    }Stop;

    typedef enum {
        STATUS_OK=0x55,
        STATUS_FAIL=0xAA
    }Status;

    explicit InitUart(CommandManager *cmd, QObject *parent = nullptr);
    ~InitUart();

    void read(const QByteArray &in) override;

public slots:
    void write(const uint32_t baudrate,
                const DataSize dSize,
                const Parity parity,
                const Stop stop) const;

signals:
    void received(const Status status, const QString &id);
};

#endif // INITUART_H
