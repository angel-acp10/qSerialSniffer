#include "GetId.h"
#include "SerialIO.h"

GetId::GetId(SerialIO *ser, QObject *parent)
    : Command{ser, parent}
{
}

GetId::~GetId()
{
}

void GetId::write() const
{
    uint16_t cmdLen = 4;
    QByteArray out;

    out.resize(cmdLen);

    // cmd start
    setHeader(out, 0x01);
    setChecksum(out);
    // cmd end

    m_ser->addRequest(out, (Command *)this);
}

void GetId::read(const QByteArray &in)
{
    if(verifyChecksum(in) == false)
        return; // checksum error

    QString id = QString(in.mid(4, 25));
    emit received(id);
}
