#include "IdDelegate.h"

#include <QPalette.h>

IdDelegate::IdDelegate(QObject *parent)
    : QStyledItemDelegate{parent},
      m_id2("2"),
      m_id3("3"),
      m_color2(QColor("red")),
      m_color3(QColor("green"))
{

}


QString IdDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    (void)locale; // unused
    bool ok;

    quint64 id = value.toUInt(&ok);
    if(!ok)
        return "";

    switch(id)
    {
    case 2:     return m_id2;
    case 3:     return m_id3;
    default:    return "";
    }
}

void IdDelegate::paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QColor background;
    bool ok;

    quint64 id = index.data().toUInt(&ok);
    if(!ok) return;

    if(id==2)       background = m_color2;
    else if(id==3)  background = m_color3;
    else            background = QColor("white");

    painter->fillRect(option.rect, background); // Paint background
    QStyledItemDelegate::paint(painter, option, index); // Paint text
}



QString IdDelegate::getId2() const
{
    return m_id2;
}

void IdDelegate::setId2(const QString id2)
{
    m_id2 = id2;
}

QString IdDelegate::getId3() const
{
    return m_id3;
}

void IdDelegate::setId3(const QString id3)
{
    m_id3 = id3;
}

QColor IdDelegate::getColor2() const
{
    return m_color2;
}

void IdDelegate::setColor2(const QColor color2)
{
    m_color2 = color2;
}

QColor IdDelegate::getColor3() const
{
    return m_color3;
}

void IdDelegate::setColor3(const QColor color3)
{
    m_color3 = color3;
}
