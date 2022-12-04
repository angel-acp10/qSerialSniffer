#include "EncodingDelegate.h"

EncodingDelegate::EncodingDelegate(QWidget *parent)
    : QStyledItemDelegate(parent),
      m_encoding(0)
{

}

QString EncodingDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    (void)locale; // unused
    QByteArray arr = value.toByteArray();

    return showAsAsciiHex(arr);
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

