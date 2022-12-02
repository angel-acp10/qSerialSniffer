#include "CommandManager.h"

CommandManager::CommandManager(SerialIO *ser, TimeStamp *tStamp0, TimeStamp *tStamp1, QObject *parent)
    : QObject{parent},
      getId(new GetId(this, this)),
      initUart(new InitUart(this, this)),
      deInitUart(new DeInitUart(this, this)),
      getAllQueue(new GetAllQueue(this, tStamp0, tStamp1, this)),
      m_ser(ser),
      m_requests(),
      m_currRequest()
{
    connect(m_ser, &SerialIO::maxAttempts, this, &CommandManager::dequeueRequest);
    connect(m_ser, &SerialIO::replyReceived, this, &CommandManager::processReply);
    connect(this, &CommandManager::sendRequest, m_ser, &SerialIO::startRequest);
}

CommandManager::~CommandManager()
{
    if(m_requests.isEmpty() == false)
        m_requests.clear();
}

void CommandManager::addRequest(QByteArray &txCmd, Command *cmdPtr)
{
    request_t request = {
        .txCmd = txCmd,
        .cmdPtr = cmdPtr
    };
    //if(m_ser->isOpen())   ///////////// to be modified
    //{
    m_requests.enqueue(request);

    if(m_ser->getStage()==SerialIO::STAGE_IDLE)
            dequeueRequest();
    //}
}

void CommandManager::dequeueRequest() // slot
{
    if(m_requests.length() > 0)
    {
        m_currRequest = m_requests.dequeue();
        emit sendRequest(m_currRequest.txCmd);
    }
}

void CommandManager::processReply(const QByteArray reply)
{
    if(verifyChecksum(reply) == false)
        emit corruptReply(); // wrong checksum

    else if(m_currRequest.txCmd[0] != reply[0])
        emit unknownReply(); // reply does not match to the requested command

    else // the corresponding command reads the reply and fires the appropiate signal
        m_currRequest.cmdPtr->read(reply);

    dequeueRequest();
}

void CommandManager::setHeader(QByteArray &buff, const uint8_t cmdIdx) const
{
    uint16_t numBytes = buff.length() - 3;

    buff[0] = (char)cmdIdx;
    buff[1] = (char)((numBytes & 0xFF00)>>8);
    buff[2] = (char)(numBytes & 0x00FF);
}

void CommandManager::setChecksum(QByteArray &buff) const
{
    uint8_t res = 0;
    int len = buff.length();

    for(int i=0; i<len-1; i++)
        res += buff[i];

    buff[len-1] = (char)~res;
}

bool CommandManager::verifyChecksum(const QByteArray &buff) const
{
    uint8_t res = 0;
    int len = buff.length();

    for(int i=0; i<len-1; i++)
        res += buff[i];

    if((char)~res == buff[len-1])
        return true;
    return false;
}
