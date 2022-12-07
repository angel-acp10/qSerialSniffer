#include "SearchWidget.h"
#include "ui_SearchWidget.h"

#include <QDebug>
#include "search/Postfix.h"
#include "search/Eval.h"

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);

    connect(ui->eval_pushButton, &QPushButton::clicked, this, &SearchWidget::evaluate);
}

SearchWidget::~SearchWidget()
{
    delete ui;
}

void SearchWidget::evaluate()
{
    QString infix = ui->plainTextEdit->toPlainText();

    std::string in_infix = infix.toStdString();

    qDebug()<<"infix: "<<in_infix.data();

    Postfix pfix;
    pfix.setInfix(in_infix);
    pfix.convert();

    std::string out_postfix = pfix.getPostfix();
    qDebug()<<"postfix: "<<out_postfix.data();

    Eval ev;
    ev.setPostfix(out_postfix);
    ev.evaluate();
    ev.getResult();

    qDebug()<<"result: "<<ev.getResult();

}
