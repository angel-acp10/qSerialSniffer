#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include "../table/FragmentsModel.h"
#include "../table/FilteredFragmentsProxy.h"
#include "ExpressionTree.h"
#include "../SettingsDialog.h"

class Search : public QObject
{
    Q_OBJECT
public:
    explicit Search(FragmentsModel *fragsModel,
                    SettingsDialog *settings,
                    QObject *parent = nullptr);

    void setFilterA(const QString& filter);
    void setFilterB(const QString& filter);
    const QString getFilterA() const;
    const QString getFilterB() const;
    FilteredFragmentsProxy *getProxyModel();

signals:
    void filterAChanged(const QString &filter);
    void filterBChanged(const QString &filter);

private:
    ExpressionTree *m_exprTreeA;
    ExpressionTree *m_exprTreeB;
    FilteredFragmentsProxy *m_filteredFrags;
};

#endif // SEARCH_H
