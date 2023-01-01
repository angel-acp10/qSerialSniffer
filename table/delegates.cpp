#include "Delegates.h"
#include <QPalette>

//------------------------
// Delegates
//------------------------
Delegates::Delegates(QAbstractTableModel *model,
                     QAbstractProxyModel *filteredModel,
                     SettingsDialog *settings,
                     QObject *parent)
    : QObject{parent},
      time(new TimeDelegate(this)),
      id(new IdDelegate(settings, this)),
      encoding(new EncodingDelegate(model, filteredModel, this))
{
}

//------------------------
// TimeDelegate
//------------------------
TimeDelegate::TimeDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}
QString TimeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    (void)locale; // unused
    bool ok;

    quint64 val = value.toUInt(&ok);
    if(!ok)
        return "";
    return usToString(val);
}

QString TimeDelegate::usToString(const quint64 val, const bool ret0)
{
    if(val == 0)
    {
        if(ret0)    return ("00:00:00:000000");
        else        return "";
    }

    quint64 aux;
    quint64 us = val % US_PER_SEC;
    aux = val / US_PER_SEC; // total_seconds
    quint64 sec = aux % SEC_PER_MIN;
    aux = aux / SEC_PER_MIN; // total_minutes
    quint64 min = aux % MIN_PER_HOUR;
    quint64 hrs = aux / MIN_PER_HOUR;

    QString str = QString("%1:%2:%3.%4")
            .arg(hrs, 2, 10, QLatin1Char('0')) // h
            .arg(min, 2, 10, QLatin1Char('0')) // m
            .arg(sec, 2, 10, QLatin1Char('0')) // s
            .arg(us, 6, 10, QLatin1Char('0')); // us

    return str;
}

//------------------------
// IdDelegate
//------------------------
IdDelegate::IdDelegate(SettingsDialog *settings, QObject *parent)
    : QStyledItemDelegate{parent},
      m_settings(settings)
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
    case 2:     return m_settings->getAliasA();
    case 3:     return m_settings->getAliasB();
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

    if(id==2)       background = m_settings->getColorA();
    else if(id==3)  background = m_settings->getColorB();
    else            background = QColor("white");

    painter->fillRect(option.rect, background); // Paint background
    QStyledItemDelegate::paint(painter, option, index); // Paint text
}

//------------------------
// EncodingDelegate
//------------------------
EncodingDelegate::EncodingDelegate(QAbstractTableModel *model,
                                   QAbstractProxyModel *filteredModel,
                                   QObject *parent)
    : QStyledItemDelegate(parent),
      m_encoding(Encoding::AsciiHex),
      m_model(model),
      m_filteredModel(filteredModel)
{
}
QString EncodingDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    (void)locale; // unused
    QByteArray arr = value.toByteArray();

    switch(m_encoding)
    {
    case Encoding::Hex:        return showAsHex(arr);
    case Encoding::Dec:        return showAsDec(arr);
    case Encoding::Bin:        return showAsBin(arr);
    case Encoding::AsciiHex:
    default:                    return showAsAsciiHex(arr);
    }
}
std::unique_ptr<QList<QString>> EncodingDelegate::getEncodings() const
{
    std::unique_ptr<QList<QString>> list(new QList<QString>);
    const QMetaObject& mo = EncodingDelegate::staticMetaObject;
    int enumIndex = mo.indexOfEnumerator("Encoding");
    QMetaEnum metaEnum = mo.enumerator(enumIndex);
    for (int i = 0; i < metaEnum.keyCount(); ++i)
        list->append(QString::fromLatin1(metaEnum.key(i)));
    return list;
}

void EncodingDelegate::showAs(const QString &encoding)
{
    const QMetaObject& mo = EncodingDelegate::staticMetaObject;
    int enumIndex = mo.indexOfEnumerator("Encoding");
    QMetaEnum metaEnum = mo.enumerator(enumIndex);
    int value = metaEnum.keyToValue(encoding.toLatin1());

    if (value == -1)
        m_encoding = Encoding::AsciiHex;
    else
        m_encoding = static_cast<Encoding>(value);

    int rows = m_model->rowCount();
    QModelIndex topL = m_model->index(0, 3);
    QModelIndex bottomR = m_model->index(rows, 3);
    emit m_model->dataChanged(topL, bottomR);

    rows = m_filteredModel->rowCount();
    topL = m_filteredModel->index(0, 3);
    bottomR = m_filteredModel->index(rows, 3);
    emit m_filteredModel->dataChanged(topL, bottomR);
}
QString EncodingDelegate::showAsAsciiHex(const QByteArray &arr) const
{
    int bytes = arr.size();
    QString fullStr = QString(4*bytes, ' '); // 1 byte represented as "ascii+hex" might need up to 4 chars
    int idxStr = 0;

    QString s;

    for(int i=0; i<bytes; i++)
    {
        if(arr[i]<0x20 || arr[i]>0x7E) // not printable
        {
            s = QString("{%1}").arg((quint8)arr[i], 2, 16, QLatin1Char('0')).toUpper();
            fullStr.replace(idxStr, 4, s);
            idxStr += 4;
        }
        else
        {
            s = QString(arr[i]);
            fullStr.replace(idxStr, 1, s);
            idxStr++;
        }
    }
    return fullStr.mid(0, idxStr);
}

QString EncodingDelegate::showAsHex(const QByteArray &arr) const
{
    return QString(arr.toHex(' ')).toUpper();
}
QString EncodingDelegate::showAsDec(const QByteArray &arr) const
{
    int bytes = arr.size();
    QString fullStr = QString(4*bytes, ' '); // 1 byte represented as dec needs 3 chars + 1 separator(space)
    QString s;

    for(int i=0; i<bytes; i++)
    {
        s = QString("%1").arg((quint8)arr[i], 3, 10, QLatin1Char('0'));
        fullStr.replace(4*i, 3, s);
    }
    return fullStr;
}
QString EncodingDelegate::showAsBin(const QByteArray &arr) const
{
    int bytes = arr.size();
    QString fullStr = QString(9*bytes, ' '); // 1 byte represented as bin needs 8 chars + 1 separator(space)
    QString s;

    for(int i=0; i<bytes; i++)
    {
        s = QString("%1").arg((quint8)arr[i], 8, 2, QLatin1Char('0'));
        fullStr.replace(9*i, 8, s);
    }
    return fullStr;
}


