#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QObject>

#include "SerialIO.h"
#include "GetId.h"


class CommandManager : public QObject
{
    Q_OBJECT
public:
    explicit CommandManager(SerialIO *ser, QObject *parent = nullptr);

    GetId *getId;

signals:

};

#endif // COMMANDMANAGER_H
