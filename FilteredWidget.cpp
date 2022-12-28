#include "FilteredWidget.h"
#include "ui_FilteredWidget.h"



FilteredWidget::FilteredWidget(FragmentsModel *fragsModel,
                               QWidget *parent)
    :   QWidget(parent),
        ui(new Ui::FilteredWidget),
        m_exprTree2(new ExpressionTree),
        m_exprTree3(new ExpressionTree)
{
    ui->setupUi(this);

    m_filteredFrags = new FilteredFragmentsProxy(m_exprTree2, m_exprTree3, this);
    m_filteredFrags->setSourceModel(fragsModel);
    ui->tableView->setModel(m_filteredFrags);
}

FilteredWidget::~FilteredWidget()
{
    delete ui;
}

void FilteredWidget::setFilter2(const QString& filter)
{
    m_exprTree2->build(filter.toStdString());
    emit filter2Changed(getFilter2());
}
void FilteredWidget::setFilter3(const QString& filter)
{
    m_exprTree3->build(filter.toStdString());
    emit filter3Changed(getFilter3());
}

const QString FilteredWidget::getFilter2() const
{
    QString filter = QString::fromStdString(m_exprTree2->toString());
    return filter;
}
const QString FilteredWidget::getFilter3() const
{
    QString filter = QString::fromStdString(m_exprTree3->toString());
    return filter;
}
