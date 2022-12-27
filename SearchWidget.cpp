#include "SearchWidget.h"
#include "ui_SearchWidget.h"

#include "search/ExpressionTree.h"


#include <QDebug>


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

    uint16_t arrayA[10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    uint8_t arrayB[10] = {100, 110, 120, 130, 140, 150, 160, 170, 180, 190};

    ExpressionTree expTree;

    ArrayNode::ArrayInfo aInfo = {
        .ptr = arrayA,
        .maxL = 10,
        .arrType = ArrayNode::ArrayInfo::kUint16
    };
    expTree.setArray("a", aInfo);
    expTree.build(in_infix);
    bool ok;
    qDebug() << expTree.toString().data() << "="<< expTree.evaluate(ok);
}
