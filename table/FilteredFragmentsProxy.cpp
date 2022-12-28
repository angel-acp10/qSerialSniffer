#include "FilteredFragmentsProxy.h"
#include "Fragment.h"

#include <QDebug>

FilteredFragmentsProxy::FilteredFragmentsProxy(ExpressionTree *exprTree2,
                                                ExpressionTree *exprTree3,
                                                QObject *parent)
    : QSortFilterProxyModel(parent),
        m_exprTree2(exprTree2),
        m_exprTree3(exprTree3)
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

    index = sourceModel()->index(sourceRow, 3, sourceParent);
    QByteArray array = sourceModel()->data(index).toByteArray();

    index = sourceModel()->index(sourceRow, 2, sourceParent);
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
        name = "portA";
        m_exprTree2->setArray(name.toStdString(), arrInfo);
        res = m_exprTree2->evaluate(ok);
        qDebug()<<"portA"<<res;
        return res;

    case Fragment::PORT_B:
        name = "portB";
        m_exprTree3->setArray(name.toStdString(), arrInfo);
        res = m_exprTree3->evaluate(ok);
        qDebug()<<"portB"<<res;
        return res;

    default:
        qDebug()<<id;
        return 0;
    }
}
