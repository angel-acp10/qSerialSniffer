#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QByteArray>
//#include "SerialIO.h"
#include <stdint.h>

class SerialIO;

class Command : public QObject
{
    Q_OBJECT

public:
    explicit Command(SerialIO *ser, QObject *parent = nullptr);
    ~Command();

    /**
     * @brief write Called from the  object. Builds the request and queues it to the
     * SerialIO requestsQueue. tx_args will vary on each command, to be implemented
     * by subclass
     * @param tx_args
     */
    // void write(tx_args) const;

    /**
     * @brief read Called from the Receiver object "CommandsIO". Analyzes the device
     * reply "in" and emits received signal
     * @param in
     */
    virtual void read(const QByteArray &in); // emits received

signals:
    /**
     * @brief rx_arguments will vary on each command, to be implemented by subclass
     */
    // void received(rx_arguments)

protected:
    void setHeader(QByteArray &buff, const uint8_t cmdIdx) const;
    void setChecksum(QByteArray &buff) const;

    bool verifyChecksum(const QByteArray &buff) const;

    SerialIO *m_ser;
};

#endif // COMMAND_H
