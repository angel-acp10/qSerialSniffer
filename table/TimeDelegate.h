#ifndef TIMEDELEGATE_H
#define TIMEDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>

class TimeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TimeDelegate(QObject *parent = nullptr);
};

#endif // TIMEDELEGATE_H
