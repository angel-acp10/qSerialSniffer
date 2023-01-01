#include "TimeDiff.h"

TimeDiff::TimeDiff(QObject *parent)
    : QObject{parent},
      m_model(new FragmentsModel(this))
{
}

FragmentsModel* TimeDiff::getModel()
{
    return m_model;
}
int TimeDiff::getFragmentsCount() const
{
    return m_model->rowCount();
}

void TimeDiff::setFirstFragment(const QModelIndex &index)
{
    std::unique_ptr<Fragment> frag = indexToFragment(index);
    m_model->reset();
    m_model->appendFragment(*frag);

    emit timeDiffChanged(0);
}
void TimeDiff::setSecondFragment(const QModelIndex &index)
{
    if(m_model->rowCount() >= 2)
        return;

    std::unique_ptr<Fragment> frag = indexToFragment(index);
    m_model->appendFragment(*frag);

    // get the first and second time and compute its difference
    QModelIndex firstFrag = m_model->index(0,0);
    quint64 firstTime = firstFrag.data().toUInt();
    if(firstTime == 0)
        firstTime = firstFrag.siblingAtColumn(1).data().toUInt();

    QModelIndex secondFrag = m_model->index(1,0);
    quint64 secondTime = secondFrag.data().toUInt();
    if(secondTime == 0)
        secondTime = secondFrag.siblingAtColumn(1).data().toUInt();

    qint64 difference = secondTime - firstTime;
    emit timeDiffChanged(difference);
}

std::unique_ptr<Fragment> TimeDiff::indexToFragment(const QModelIndex &index)
{
    // read model data
    quint64 start = index.siblingAtColumn(0).data().toUInt();
    quint64 end = index.siblingAtColumn(1).data().toUInt();
    Fragment::Port port = static_cast<Fragment::Port>(index.siblingAtColumn(2).data().toUInt());
    QByteArray data = index.siblingAtColumn(3).data().toByteArray();

    std::unique_ptr<Fragment> frag (new Fragment);

    // index column could be 0 (start time was selected) or 1 (end time was selected)
    // in the timediff model only the selected time will be shown.
    if(index.column() == 0)
    {
        frag->setPort(port);
        frag->setStartAcumUs(start);
        frag->setEndAcumUs(0);
        frag->setData(data);
    }
    else if(index.column() == 1)
    {
        frag->setPort(port);
        frag->setStartAcumUs(0);
        frag->setEndAcumUs(end);
        frag->setData(data);
    }
    else
    {
        Q_ASSERT(false);
    }
    return frag;
}
