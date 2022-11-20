#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QObject>
#include <QQueue>

#include "SerialIO.h"
#include "Command.h"
#include "GetId.h"
#include "InitUart.h"


class CommandManager : public QObject
{
    Q_OBJECT
public:
    explicit CommandManager(SerialIO *ser, QObject *parent = nullptr);
    ~CommandManager();

    // commands
    GetId *getId;
    InitUart *initUart;

    // auxiliar functions for "Commands" subclasses
    void setHeader(QByteArray &buff, const uint8_t cmdIdx) const;
    void setChecksum(QByteArray &buff) const;

public slots:
    void addRequest(QByteArray &txCmd, Command *cmdPtr);
    void dequeueRequest();
    void processReply(const QByteArray reply);

signals:
    void sendRequest(QByteArray &request);
    void corruptReply();
    void unknownReply();

private:
    typedef struct{
        QByteArray txCmd;
        Command *cmdPtr;
    }request_t;

    SerialIO *m_ser;
    QQueue<request_t> m_requests;
    request_t m_currRequest;

    bool verifyChecksum(const QByteArray &buff) const;
};

#endif // COMMANDMANAGER_H
