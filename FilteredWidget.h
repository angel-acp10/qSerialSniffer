#ifndef FILTEREDWIDGET_H
#define FILTEREDWIDGET_H

#include <QWidget>

namespace Ui {
class FilteredWidget;
}

class FilteredWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilteredWidget(QWidget *parent = nullptr);
    ~FilteredWidget();

private:
    Ui::FilteredWidget *ui;
};

#endif // FILTEREDWIDGET_H
