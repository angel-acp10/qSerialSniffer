#include "SearchWidget.h"
#include "ui_SearchWidget.h"

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);
}

SearchWidget::~SearchWidget()
{
    delete ui;
}
