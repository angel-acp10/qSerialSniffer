#ifndef GETID_H
#define GETID_H

#include "Command.h"

class GetId : public Command
{
    Q_OBJECT
public:
    enum Status{
        STATUS_OK=0x55
    };

    explicit GetId(CommandManager *cmd, QObject *parent = nullptr);
    ~GetId();

    void read(const QByteArray &in) override;

public slots:
    void write() const;

signals:
    void received(const GetId::Status status, const QString &id);
};

#endif // GETID_H
