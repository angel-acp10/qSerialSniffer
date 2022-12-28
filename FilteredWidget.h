#ifndef FILTEREDWIDGET_H
#define FILTEREDWIDGET_H

#include <QWidget>
#include "table/FragmentsModel.h"
#include "table/FilteredFragmentsProxy.h"
#include "search/ExpressionTree.h"

namespace Ui {
class FilteredWidget;
}

class FilteredWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilteredWidget(FragmentsModel *fragsModel,
                            QWidget *parent = nullptr);
    ~FilteredWidget();

    void setFilterA(const QString& filter);
    void setFilterB(const QString& filter);
    const QString getFilterA() const;
    const QString getFilterB() const;

signals:
    void filterAChanged(const QString &filter);
    void filterBChanged(const QString &filter);

private:
    Ui::FilteredWidget *ui;

    ExpressionTree *m_exprTreeA;
    ExpressionTree *m_exprTreeB;
    FilteredFragmentsProxy *m_filteredFrags;
};

#endif // FILTEREDWIDGET_H
