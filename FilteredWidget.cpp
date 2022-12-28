#include "FilteredWidget.h"
#include "ui_FilteredWidget.h"



FilteredWidget::FilteredWidget(FragmentsModel *fragsModel,
                               QWidget *parent)
    :   QWidget(parent),
        ui(new Ui::FilteredWidget),
        m_exprTreeA(new ExpressionTree),
        m_exprTreeB(new ExpressionTree)
{
    ui->setupUi(this);

    m_filteredFrags = new FilteredFragmentsProxy(m_exprTreeA, m_exprTreeB, this);
    m_filteredFrags->setSourceModel(fragsModel);
    ui->tableView->setModel(m_filteredFrags);
}

FilteredWidget::~FilteredWidget()
{
    delete ui;
}

void FilteredWidget::setFilterA(const QString& filter)
{
    m_exprTreeA->build(filter.toStdString());
    m_filteredFrags->invalidate();
    emit filterAChanged(getFilterA());
}
void FilteredWidget::setFilterB(const QString& filter)
{
    m_exprTreeB->build(filter.toStdString());
    m_filteredFrags->invalidate();
    emit filterBChanged(getFilterB());
}

const QString FilteredWidget::getFilterA() const
{
    QString filter = QString::fromStdString(m_exprTreeA->toString());
    return filter;
}
const QString FilteredWidget::getFilterB() const
{
    QString filter = QString::fromStdString(m_exprTreeB->toString());
    return filter;
}
