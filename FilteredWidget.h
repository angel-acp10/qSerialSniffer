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

    void setFilter2(const QString& filter);
    void setFilter3(const QString& filter);
    const QString getFilter2() const;
    const QString getFilter3() const;

signals:
    void filter2Changed(const QString &filter);
    void filter3Changed(const QString &filter);

private:
    Ui::FilteredWidget *ui;

    ExpressionTree *m_exprTree2;
    ExpressionTree *m_exprTree3;
    FilteredFragmentsProxy *m_filteredFrags;
};

#endif // FILTEREDWIDGET_H
