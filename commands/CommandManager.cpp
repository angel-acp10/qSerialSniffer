#include "CommandManager.h"

CommandManager::CommandManager(SerialIO *ser, QObject *parent)
    : QObject{parent},
      getId(new GetId(ser, parent))

{
}
