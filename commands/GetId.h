#ifndef GETID_H
#define GETID_H

#include "Command.h"

class GetId : public Command
{
    Q_OBJECT
public:
    explicit GetId(SerialIO *ser, QObject *parent = nullptr);
    ~GetId();

    void write() const;
    void read(const QByteArray &in) override;

signals:
    void received(const QString &id);
};

#endif // GETID_H
