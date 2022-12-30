#include "Search.h"

Search::Search(FragmentsModel *fragsModel,
               SettingsDialog *settings,
               QObject *parent)
    : QObject{parent},
      m_exprTreeA(new ExpressionTree),
      m_exprTreeB(new ExpressionTree)
{
    m_filteredFrags = new FilteredFragmentsProxy(m_exprTreeA,
                                                 m_exprTreeB,
                                                 settings,
                                                 this);
    m_filteredFrags->setSourceModel(fragsModel);
}

void Search::setFilterA(const QString& filter)
{
    m_exprTreeA->build(filter.toStdString());
    m_filteredFrags->invalidate();
    emit filterAChanged(getFilterA());
}
void Search::setFilterB(const QString& filter)
{
    m_exprTreeB->build(filter.toStdString());
    m_filteredFrags->invalidate();
    emit filterBChanged(getFilterB());
}
const QString Search::getFilterA() const
{
    QString filter = QString::fromStdString(m_exprTreeA->toString());
    return filter;
}
const QString Search::getFilterB() const
{
    QString filter = QString::fromStdString(m_exprTreeB->toString());
    return filter;
}

FilteredFragmentsProxy* Search::getProxyModel()
{
    return m_filteredFrags;
}
