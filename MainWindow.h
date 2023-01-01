#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include "SettingsDialog.h"
#include "commands/SerialIO.h"
#include "commands/CommandManager.h"
#include "TimeStamp.h"
#include <QTimer>

#include "table/FragmentsModel.h"
#include "table/Delegates.h"
#include "search/Search.h"
#include "table/TimeDiff.h"

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


private slots:
    void play();
    void pause();

private:
    Ui::MainWindow *ui;

    SettingsDialog *mSettingsDialog;
    SerialIO *mSerial;
    TimeStamp *mTStamp0;
    TimeStamp *mTStamp1;
    CommandManager *mCmds;
    FragmentsModel *mFragModel;
    Delegates *mDelegates;
    Search *mSearch;
    TimeDiff *mTimeDiff;
    QTimer *mTimer;

};
#endif // MAINWINDOW_H
