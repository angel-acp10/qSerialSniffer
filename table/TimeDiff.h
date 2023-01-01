#ifndef TIMEDIFF_H
#define TIMEDIFF_H

#include <QObject>
#include "FragmentsModel.h"
#include "Fragment.h"
#include <memory>

class TimeDiff : public QObject
{
    Q_OBJECT
public:
    explicit TimeDiff(QObject *parent = nullptr);
    FragmentsModel *getModel();
    int getFragmentsCount() const;

signals:
    void timeDiffChanged(const qint64 usDifference);

public slots:
    void setFirstFragment(const QModelIndex &index);
    void setSecondFragment(const QModelIndex &index);

private:
    FragmentsModel *m_model;

    std::unique_ptr<Fragment> indexToFragment(const QModelIndex &index);
};

#endif // TIMEDIFF_H
