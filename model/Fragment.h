#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <qglobal.h>
#include <QByteArray>

class Fragment
{
public:
    enum Port {
        PORT_A=0x02,
        PORT_B=0x03
    };

    Fragment();
    ~Fragment();

    Port getPort() const;
    void setPort(const Port port);

    quint64 getStartAcumUs() const;
    void setStartAcumUs(const quint64 startAcumUs);

    quint64 getEndAcumUs() const;
    void setEndAcumUs(const quint64 endAcumUs);

    QByteArray getData() const;
    void setData(const QByteArray& data);


private:
    Port m_port;
    quint64 m_startAcumUs;
    quint64 m_endAcumUs;
    QByteArray m_data;
};

#endif // FRAGMENT_H
