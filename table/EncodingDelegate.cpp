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

    return showAsBin(arr);
}

QString EncodingDelegate::showAsHex(const QByteArray &arr) const
{
    int bytes = arr.size();
    QString fullStr = QString(3*bytes, ' '); // 1 byte represented as hex needs 2 chars + 1 separator(space)
    QString s;

    for(int i=0; i<bytes; i++)
    {
        s = QString("%1").arg(arr[i], 2, 16, QLatin1Char('0'));
        fullStr.replace(3*i, 2, s);
    }
    return fullStr;
}

QString EncodingDelegate::showAsDec(const QByteArray &arr) const
{
    int bytes = arr.size();
    QString fullStr = QString(4*bytes, ' '); // 1 byte represented as dec needs 3 chars + 1 separator(space)
    QString s;

    for(int i=0; i<bytes; i++)
    {
        s = QString("%1").arg(arr[i], 3, 10, QLatin1Char('0'));
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
        s = QString("%1").arg(arr[i], 8, 2, QLatin1Char('0'));
        fullStr.replace(9*i, 8, s);
    }
    return fullStr;
}

