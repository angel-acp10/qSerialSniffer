#include "SearchWidget.h"
#include "ui_SearchWidget.h"

#include <QDebug>


SearchWidget::SearchWidget(FilteredWidget *filteredWidget, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget),
    m_filteredWidget(filteredWidget)
{
    ui->setupUi(this);

    connect(ui->set2_pushButton, &QPushButton::clicked,
            this, [this]()
            {
                QString filter = ui->edit2_plainTextEdit->toPlainText();
                m_filteredWidget->setFilter2(filter);
            }
    );
    connect(ui->set3_pushButton, &QPushButton::clicked,
            this, [this]()
            {
                QString filter = ui->edit3_plainTextEdit->toPlainText();
                m_filteredWidget->setFilter3(filter);
            }
    );
    connect(m_filteredWidget, &FilteredWidget::filter2Changed,
            ui->curr2_plainTextEdit, &QPlainTextEdit::setPlainText);
    connect(m_filteredWidget, &FilteredWidget::filter3Changed,
            ui->curr3_plainTextEdit, &QPlainTextEdit::setPlainText);
}

SearchWidget::~SearchWidget()
{
    delete ui;
}
