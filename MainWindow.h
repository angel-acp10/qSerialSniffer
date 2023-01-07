#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SettingsDialog.h"

#include "commands/CommWorker.h"

#include "table/FragmentsModel.h"
#include "table/Delegates.h"
#include "search/Search.h"
#include "table/TimeDiff.h"
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initTopPortLabels();
    void initLeftToolbar();
    void initEncodingList();
    void initTable();
    void initRightFilteredTable();
    void initBottomSearch();
    void initBottomTimeDiff();

    void initHideColumn_check(QCheckBox * checkBox,
                              FragmentsModel::Column column,
                              bool hide);
    void initAutoScroll_check();

private:
    Ui::MainWindow *ui;

    SettingsDialog *mSettingsDialog;
    FragmentsModel *mFragModel;
    CommWorker *mCommWorker;
    Delegates *mDelegates;
    Search *mSearch;
    TimeDiff *mTimeDiff;
    QTimer *mTimer;

};
#endif // MAINWINDOW_H
