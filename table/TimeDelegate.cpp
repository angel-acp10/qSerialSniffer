#include "TimeDelegate.h"

const quint64 US_PER_SEC = 1000000;
const quint64 SEC_PER_MIN = 60;
const quint64 MIN_PER_HOUR = 60;

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
