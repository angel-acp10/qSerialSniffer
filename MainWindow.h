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
#include "table/TimeDelegate.h"
#include "table/IdDelegate.h"
#include "table/EncodingDelegate.h"

#include "SearchWidget.h"
#include "FilteredWidget.h"

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
    void initButtons();
    void initEncodingList();
    void initTableWidget();
    void initDocks();

private slots:
    void applySettings(const QString &port,
                       const int bauds, const QString &parity, const int dataSize,
                       const QColor &colorA, const QColor &colorB,
                       const QString &aliasA, const QString &aliasB);
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
    TimeDelegate *mTimeDelegate;
    IdDelegate *mIdDelegate;
    EncodingDelegate *mEncDelegate;
    QTimer *mTimer;

    QDockWidget *mSearchDock;
    QDockWidget *mFilteredDock;
    SearchWidget *mSearchWidget;
    FilteredWidget *mFilteredWidget;
    
    QString mPort;
    int mSnifferBaudrate;
    QString mSnifferParity;
    int mSnifferDataSize;
    QString mAliasA;
    QString mAliasB;
    QColor mColorA;
    QColor mColorB;

};
#endif // MAINWINDOW_H
