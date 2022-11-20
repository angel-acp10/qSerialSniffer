#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QByteArray>
#include <stdint.h>

class CommandManager;

class Command : public QObject
{
    Q_OBJECT

public:
    explicit Command(CommandManager *cmd, QObject *parent = nullptr);
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
    CommandManager *m_cmd;
};

#endif // COMMAND_H
