#ifndef FilteredFragmentsProxy_H
#define FilteredFragmentsProxy_H

#include <QSortFilterProxyModel>
#include <QObject>
#include "../search/ExpressionTree.h"
#include "../SettingsDialog.h"

class FilteredFragmentsProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FilteredFragmentsProxy(ExpressionTree *exprTreeA,
                           ExpressionTree *exprTreeB,
                           SettingsDialog *settings,
                           QObject *parent = nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    ExpressionTree *m_exprTreeA;
    ExpressionTree *m_exprTreeB;
    SettingsDialog *m_settings;
};

#endif // FilteredFragmentsProxy_H
