#include "Command.h"
#include "CommandManager.h"

Command::Command(CommandManager *cmd, QObject *parent)
    : QObject{parent},
      m_cmd(cmd)
{
}

Command::~Command()
{
}

void Command::read(const QByteArray &in)
{
    (void)in; // unused
}


