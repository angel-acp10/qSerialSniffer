#ifndef FilteredFragmentsProxy_H
#define FilteredFragmentsProxy_H

#include <QSortFilterProxyModel>
#include <QObject>
#include "../search/ExpressionTree.h"

class FilteredFragmentsProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FilteredFragmentsProxy(ExpressionTree *exprTree2,
                           ExpressionTree *exprTree3,
                           QObject *parent = nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    ExpressionTree *m_exprTree2;
    ExpressionTree *m_exprTree3;
};

#endif // FilteredFragmentsProxy_H
