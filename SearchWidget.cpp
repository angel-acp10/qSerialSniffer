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
    Postfix pfix;
    Eval ev;

    uint16_t arrayA[10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    uint8_t arrayB[10] = {100, 110, 120, 130, 140, 150, 160, 170, 180, 190};

    QString infix = ui->plainTextEdit->toPlainText();

    std::string in_infix = infix.toStdString();

    qDebug()<<"infix: "<<in_infix.data();


    pfix.setInfix(in_infix);
    pfix.convert();

    std::string out_postfix = pfix.getPostfix();
    qDebug()<<"postfix: "<<out_postfix.data();

    ev.clearArrayMap();
    ev.addToArrayMap("a", arrayA, Eval::VarType::TYPE_UINT16, 10);
    ev.addToArrayMap("b", arrayB, Eval::VarType::TYPE_UINT8, 10);
    ev.setPostfix(out_postfix);
    ev.evaluate();
    ev.getResult();

    qDebug()<<"result: "<<ev.getResult();

}
