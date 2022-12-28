#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include "FilteredWidget.h"
#include "SettingsDialog.h"

namespace Ui {
class SearchWidget;
}

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(FilteredWidget *filteredWidget,
                          SettingsDialog *settings,
                          QWidget *parent = nullptr);
    ~SearchWidget();

private:
    Ui::SearchWidget *ui;
    FilteredWidget *m_filteredWidget;
    SettingsDialog *m_settings;
};

#endif // SEARCHWIDGET_H
