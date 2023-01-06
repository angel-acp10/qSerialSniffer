#include "FilteredFragmentsProxy.h"
#include "Fragment.h"
#include "FragmentsModel.h"

#include <QDebug>

FilteredFragmentsProxy::FilteredFragmentsProxy(ExpressionTree *exprTreeA,
                                                ExpressionTree *exprTreeB,
                                                SettingsDialog *settings,
                                                QObject *parent)
    : QSortFilterProxyModel(parent),
        m_exprTreeA(exprTreeA),
        m_exprTreeB(exprTreeB),
        m_settings(settings)
{

}

bool FilteredFragmentsProxy::filterAcceptsRow(
        int sourceRow,
        const QModelIndex
        &sourceParent) const
{
    QModelIndex index;
    QString name;
    bool ok;

    index = sourceModel()->index(sourceRow, FragmentsModel::Column::kData, sourceParent);
    QByteArray array = sourceModel()->data(index, FragmentsModel::RawDataRole).toByteArray();

    index = sourceModel()->index(sourceRow, FragmentsModel::Column::kId, sourceParent);
    int id = sourceModel()->data(index).toInt();

    ArrayNode::ArrayInfo arrInfo = {
        .ptr = array.data(),
        .maxL = static_cast<int>(array.size()),
        .arrType = ArrayNode::ArrayInfo::kUint8
    };

    int res;
    switch(id)
    {
    case Fragment::PORT_A:
        name = m_settings->getAliasA();
        m_exprTreeA->setArray(name.toStdString(), arrInfo);
        res = m_exprTreeA->evaluate(ok);
        qDebug()<<name<<res;
        return res;

    case Fragment::PORT_B:
        name = m_settings->getAliasB();
        m_exprTreeB->setArray(name.toStdString(), arrInfo);
        res = m_exprTreeB->evaluate(ok);
        qDebug()<<name<<res;
        return res;

    default:
        qDebug()<<id;
        return 0;
    }
}
