#include "SearchWidget.h"
#include "ui_SearchWidget.h"

#include <QDebug>


SearchWidget::SearchWidget(FilteredWidget *filteredWidget,
                           SettingsDialog *settings,
                           QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget),
    m_filteredWidget(filteredWidget),
    m_settings(settings)
{
    ui->setupUi(this);

    connect(ui->setA_pushButton, &QPushButton::clicked,
            this, [this]()
            {
                QString filter = ui->editA_plainTextEdit->toPlainText();
                m_filteredWidget->setFilterA(filter);
            }
    );
    connect(ui->setB_pushButton, &QPushButton::clicked,
            this, [this]()
            {
                QString filter = ui->editB_plainTextEdit->toPlainText();
                m_filteredWidget->setFilterB(filter);
            }
    );
    connect(m_filteredWidget, &FilteredWidget::filterAChanged,
            ui->currA_plainTextEdit, &QPlainTextEdit::setPlainText);
    connect(m_filteredWidget, &FilteredWidget::filterBChanged,
            ui->currB_plainTextEdit, &QPlainTextEdit::setPlainText);
}

SearchWidget::~SearchWidget()
{
    delete ui;
}
