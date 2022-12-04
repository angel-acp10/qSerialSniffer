#include "FilteredWidget.h"
#include "ui_FilteredWidget.h"

FilteredWidget::FilteredWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilteredWidget)
{
    ui->setupUi(this);
}

FilteredWidget::~FilteredWidget()
{
    delete ui;
}
