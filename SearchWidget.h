#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include "FilteredWidget.h"

namespace Ui {
class SearchWidget;
}

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(FilteredWidget *filteredWidget, QWidget *parent = nullptr);
    ~SearchWidget();

private:
    Ui::SearchWidget *ui;
    FilteredWidget *m_filteredWidget;
};

#endif // SEARCHWIDGET_H
