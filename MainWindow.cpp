#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMetaEnum>
#include <QMessageBox>
#include <QDateTime>
#include <QFontDatabase>
#include "table/Fragment.h"
#include <QDebug>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mSettingsDialog(new SettingsDialog(this)),
      mSerial(new SerialIO(100, 3, this)), ///////// to be modified
      mTStamp0(new TimeStamp(this)),
      mTStamp1(new TimeStamp(this)),
      mCmds(new CommandManager(mSerial, mTStamp0, mTStamp1, this)),
      mFragModel(new FragmentsModel(this)),
      mSearch(new Search(mFragModel, mSettingsDialog, this)),
      mTimeDiff(new TimeDiff(this)),
      mTimer(new QTimer)
{
    ui->setupUi(this);

    mDelegates = new Delegates(mFragModel,
                               mSearch->getProxyModel(),
                               mSettingsDialog,
                               this);

    mSerial->start(); ///////// to be modified

    initTopPortLabels();
    initLeftToolbar();
    initEncodingList();
    initTable();
    initRightFilteredTable();
    initBottomSearch();
    initBottomTimeDiff();

    initHideColumn_check(ui->number_checkBox,
                         FragmentsModel::Column::kNumber,
                         false);
    initHideColumn_check(ui->start_checkBox,
                         FragmentsModel::Column::kStart,
                         true);
    initHideColumn_check(ui->end_checkBox,
                         FragmentsModel::Column::kEnd,
                         true);
    initAutoScroll_check();


    connect(mCmds->getId, &GetId::received,
            this, [this](GetId::Status, QString id)
            {
                QMessageBox::information(this, "GetId response", id);
            });

    /*
    connect(mSerial, &SerialIO::maxAttempts,
            [this]()
            {
                QMessageBox::critical(this, "SerialPort maxxAttempts", "");
            });
*/
    connect(mCmds->getAllQueue, &GetAllQueue::received,
            this, [this](QList<Fragment> lst)
            {
                for(auto &frag : lst)
                {
                    frag.setNumber(mFragModel->rowCount());
                    mFragModel->appendFragment(frag);
                }
            });

    connect(mTimer, &QTimer::timeout, mCmds->getAllQueue, &GetAllQueue::write);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTopPortLabels()
{
    connect(mSettingsDialog, &SettingsDialog::aliasAChanged,
            this, [this](const QString& aliasA)
            {
                ui->portA_label->setText(QString("Port A (%1)").arg(aliasA));
            }
    );
    connect(mSettingsDialog, &SettingsDialog::aliasBChanged,
            this, [this](const QString& aliasB)
            {
                ui->portB_label->setText(QString("Port B (%1)").arg(aliasB));
            }
    );
    connect(mSettingsDialog, &SettingsDialog::colorAChanged,
            this, [this](const QColor& colorA)
            {
                QString styleSheetA = QString("QLabel {color : %1}").arg(colorA.name());
                ui->portA_label->setStyleSheet(styleSheetA);
            }
    );
    connect(mSettingsDialog, &SettingsDialog::colorBChanged,
            this, [this](const QColor& colorB)
            {
                QString styleSheetB = QString("QLabel {color : %1}").arg(colorB.name());
                ui->portB_label->setStyleSheet(styleSheetB);
            }
    );
}

void MainWindow::initLeftToolbar()
{
    ui->pause_toolButton->setDisabled(true);
    ui->play_toolButton->setEnabled(true);
    ui->settings_toolButton->setEnabled(true);

    connect(ui->settings_toolButton, &QToolButton::clicked,
                mSettingsDialog, &SettingsDialog::exec);

    connect(ui->play_toolButton, &QToolButton::clicked, this, &MainWindow::play);
    connect(ui->play_toolButton, &QToolButton::clicked, mTStamp0, &TimeStamp::onPlayClicked);
    connect(ui->play_toolButton, &QToolButton::clicked, mTStamp1, &TimeStamp::onPlayClicked);

    connect(ui->pause_toolButton, &QToolButton::clicked, this, &MainWindow::pause);
    connect(ui->pause_toolButton, &QToolButton::clicked, mTStamp0, &TimeStamp::onPauseClicked);
    connect(ui->pause_toolButton, &QToolButton::clicked, mTStamp1, &TimeStamp::onPauseClicked);

    connect(ui->reset_toolButton, &QToolButton::clicked, mFragModel, &FragmentsModel::reset);

    ui->bottom_stackedWidget->setVisible(false);

    ui->filteredTerminal_toolButton->setCheckable(true);
    ui->filteredTerminal_toolButton->setChecked(false);
    ui->search_toolButton->setCheckable(true);
    ui->search_toolButton->setChecked(false);
    ui->timeDiff_toolButton->setCheckable(true); 
    ui->timeDiff_toolButton->setChecked(false);
    
    // filtered monitor
    ui->filteredMonitor_widget->setVisible(false);
    connect(ui->filteredTerminal_toolButton, &QToolButton::clicked,
            ui->filteredMonitor_widget, &QWidget::setVisible);


    connect(ui->search_toolButton, &QToolButton::clicked,
            this, [this](bool checked)
            {
                // display corresponfing widget on stacked widget
                int idx = ui->bottom_stackedWidget->indexOf(ui->search_widgetPage);
                ui->bottom_stackedWidget->setCurrentIndex(idx);

                // change stacked widget visibility
                ui->bottom_stackedWidget->setVisible(checked);

                // uncheck any other toolbutton whose associated widget
                // is also in stacked widget
                ui->timeDiff_toolButton->setChecked(false);
            }
    );

    connect(ui->timeDiff_toolButton, &QToolButton::clicked,
            this, [this](bool checked)
            {
                // display corresponfing widget on stacked widget
                int idx = ui->bottom_stackedWidget->indexOf(ui->timeDiff_widgetPage);
                ui->bottom_stackedWidget->setCurrentIndex(idx);

                // change stacked widget visibility
                ui->bottom_stackedWidget->setVisible(checked);

                // uncheck any other toolbutton whose associated widget
                // is also in stacked widget
                ui->search_toolButton->setChecked(false);
            }
    );

}

void MainWindow::initEncodingList()
{
    std::unique_ptr<QList<QString>> encodingList(
                mDelegates->encoding->getEncodings() );

    ui->encoding_comboBox->addItems(*encodingList);

    connect(ui->encoding_comboBox, &QComboBox::currentTextChanged,
            mDelegates->encoding, &EncodingDelegate::showAs);
}

void MainWindow::initTable()
{
    ui->tableView->setTimeDifferenceObject(mTimeDiff);

    ui->tableView->setItemDelegateForColumn(FragmentsModel::Column::kStart, mDelegates->time);
    ui->tableView->setItemDelegateForColumn(FragmentsModel::Column::kEnd, mDelegates->time);
    ui->tableView->setItemDelegateForColumn(FragmentsModel::Column::kId, mDelegates->id);
    ui->tableView->setItemDelegateForColumn(FragmentsModel::Column::kData, mDelegates->encoding);

    ui->tableView->setModel(mFragModel);

    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    QFont fixedFont("Monospace");
    fixedFont.setStyleHint(QFont::TypeWriter);
    ui->tableView->setFont(fixedFont);

    int idx = ui->horizontal_splitter->indexOf(ui->monitor_widget);
    ui->horizontal_splitter->setCollapsible(idx, false);

    idx = ui->vertical_splitter->indexOf(ui->central_widget);
    ui->vertical_splitter->setCollapsible(idx, false);

    // one time connection to adjust time and id columns the first time data is received
    QMetaObject::Connection * const connection = new QMetaObject::Connection;
    *connection = connect(mFragModel, &QAbstractItemModel::rowsInserted,
            this, [this, connection] ()
            {
                QObject::disconnect(*connection);
                for(int i=0; i<mFragModel->columnCount(); i++)
                    ui->tableView->resizeColumnToContents(i);
            }
    );
}

void MainWindow::initRightFilteredTable()
{
    ui->filtered_tableView->setTimeDifferenceObject(mTimeDiff);

    ui->filtered_tableView->setItemDelegateForColumn(FragmentsModel::Column::kStart, mDelegates->time);
    ui->filtered_tableView->setItemDelegateForColumn(FragmentsModel::Column::kEnd, mDelegates->time);
    ui->filtered_tableView->setItemDelegateForColumn(FragmentsModel::Column::kId, mDelegates->id);
    ui->filtered_tableView->setItemDelegateForColumn(FragmentsModel::Column::kData, mDelegates->encoding);

    ui->filtered_tableView->setModel(mSearch->getProxyModel());

    ui->filtered_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->filtered_tableView->horizontalHeader()->setStretchLastSection(true);

    QFont fixedFont("Monospace");
    fixedFont.setStyleHint(QFont::TypeWriter);
    ui->filtered_tableView->setFont(fixedFont);

    int idx = ui->horizontal_splitter->indexOf(ui->filteredMonitor_widget);
    ui->horizontal_splitter->setCollapsible(idx, false);

    // one time connection to adjust time and id columns the first time data is received
    QMetaObject::Connection * const connection = new QMetaObject::Connection;
    *connection = connect(mSearch->getProxyModel(), &QAbstractItemModel::rowsInserted,
            this, [this, connection] ()
            {
                QObject::disconnect(*connection);
                for(int i=0; i<mSearch->getProxyModel()->columnCount(); i++)
                    ui->filtered_tableView->resizeColumnToContents(i);
            }
    );
}

void MainWindow::initBottomSearch()
{
    connect(ui->setA_pushButton, &QPushButton::clicked,
            this, [this]()
            {
                QString filter = ui->editA_plainTextEdit->toPlainText();
                mSearch->setFilterA(filter);
            }
    );
    connect(ui->setB_pushButton, &QPushButton::clicked,
            this, [this]()
            {
                QString filter = ui->editB_plainTextEdit->toPlainText();
                mSearch->setFilterB(filter);
            }
    );
    connect(mSearch, &Search::filterAChanged,
            ui->currA_plainTextEdit, &QPlainTextEdit::setPlainText);
    connect(mSearch, &Search::filterBChanged,
            ui->currB_plainTextEdit, &QPlainTextEdit::setPlainText);

    connect(mSettingsDialog, &SettingsDialog::aliasAChanged,
            this, [this](const QString& aliasA)
            {
                ui->searchPortA_label->setText(QString("Port A (%1)").arg(aliasA));
            }
    );
    connect(mSettingsDialog, &SettingsDialog::aliasBChanged,
            this, [this](const QString& aliasB)
            {
                ui->searchPortB_label->setText(QString("Port B (%1)").arg(aliasB));
            }
    );
    connect(mSettingsDialog, &SettingsDialog::colorAChanged,
            this, [this](const QColor& colorA)
            {
                QString styleSheetA = QString("QLabel {color : %1}").arg(colorA.name());
                ui->searchPortA_label->setStyleSheet(styleSheetA);
            }
    );
    connect(mSettingsDialog, &SettingsDialog::colorBChanged,
            this, [this](const QColor& colorB)
            {
                QString styleSheetB = QString("QLabel {color : %1}").arg(colorB.name());
                ui->searchPortB_label->setStyleSheet(styleSheetB);
            }
    );

    int centralIdx = ui->vertical_splitter->indexOf(ui->central_widget);
    int bottomStackedIdx = ui->vertical_splitter->indexOf(ui->bottom_stackedWidget);
    ui->vertical_splitter->setCollapsible(bottomStackedIdx, false);

    ui->vertical_splitter->setStretchFactor(centralIdx, 10);
    ui->vertical_splitter->setStretchFactor(bottomStackedIdx, 1);
}

void MainWindow::initBottomTimeDiff()
{
    ui->timeDiff_label->setText("Time difference");
    // time difference label
    connect(mTimeDiff, &TimeDiff::timeDiffChanged,
            this, [this](const qint64 usDiff)
            {
                QString hourMinSecMicros = TimeDelegate::usToString(abs(usDiff), true);
                QString millis = QString("%1.%2 ms")
                        .arg(abs(usDiff)/1000)
                        .arg(abs(usDiff)%1000, 3, 10, QLatin1Char('0'));
                if(usDiff < 0)
                {
                    hourMinSecMicros = "-" + hourMinSecMicros;
                    millis = "-" + millis;
                }
                ui->timeDiff_label->setText( QString("Time difference is: %1 (%2)")
                                            .arg(millis, hourMinSecMicros) );
            }
    );

    // connect tableview signals
    connect(ui->tableView, &FragmentsView::selectForCompare,
            mTimeDiff, &TimeDiff::setFirstFragment);
    connect(ui->tableView, &FragmentsView::compareWithSelected,
            mTimeDiff, &TimeDiff::setSecondFragment);

    // connect filtered tableview signals
    connect(ui->filtered_tableView, &FragmentsView::selectForCompare,
            mTimeDiff, &TimeDiff::setFirstFragment);
    connect(ui->filtered_tableView, &FragmentsView::selectForCompare,
            mTimeDiff, &TimeDiff::setFirstFragment);

    ui->timeDiff_tableView->setItemDelegateForColumn(FragmentsModel::Column::kStart, mDelegates->time);
    ui->timeDiff_tableView->setItemDelegateForColumn(FragmentsModel::Column::kEnd, mDelegates->time);
    ui->timeDiff_tableView->setItemDelegateForColumn(FragmentsModel::Column::kId, mDelegates->id);
    ui->timeDiff_tableView->setItemDelegateForColumn(FragmentsModel::Column::kData, mDelegates->encoding);

    ui->timeDiff_tableView->setModel(mTimeDiff->getModel());

    ui->timeDiff_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->timeDiff_tableView->horizontalHeader()->setStretchLastSection(true);

    QFont fixedFont("Monospace");
    fixedFont.setStyleHint(QFont::TypeWriter);
    ui->timeDiff_tableView->setFont(fixedFont);

    // connection to adjust time and id columns
    connect(mTimeDiff->getModel(), &QAbstractItemModel::rowsInserted,
            this, [this] ()
            {
                for(int i=0; i<mTimeDiff->getModel()->columnCount(); i++)
                    ui->timeDiff_tableView->resizeColumnToContents(i);
            }
    );
}

void MainWindow::initHideColumn_check(QCheckBox * checkBox,
                                      FragmentsModel::Column column,
                                      bool hide)
{
    if(hide)
    {
        checkBox->setCheckState(Qt::Unchecked);
        ui->tableView->hideColumn(column);
        ui->filtered_tableView->hideColumn(column);
    }
    else
    {
        checkBox->setCheckState(Qt::Checked);
        ui->tableView->showColumn(column);
        ui->filtered_tableView->showColumn(column);
    }

    connect(checkBox, &QCheckBox::stateChanged,
            this, [this, column] (int state)
            {
                switch(static_cast<Qt::CheckState>(state))
                {
                case Qt::Checked:
                    ui->tableView->showColumn(column);
                    ui->filtered_tableView->showColumn(column);
                    break;
                case Qt::PartiallyChecked:
                case Qt::Unchecked:
                    ui->tableView->hideColumn(column);
                    ui->filtered_tableView->hideColumn(column);
                    break;
                }
            }
    );
}

void MainWindow::initAutoScroll_check()
{
    ui->autoscoll_checkBox->setCheckState(Qt::Checked);

    // keep qtableview scrolled to the last inserted row
    connect(mFragModel, &QAbstractItemModel::rowsInserted,
            this, [this]()
            {
                if(ui->autoscoll_checkBox->isChecked())
                    ui->tableView->scrollToBottom();
            }
    );
    connect(mSearch->getProxyModel(), &QAbstractItemModel::rowsInserted,
            this, [this]()
            {
                if(ui->autoscoll_checkBox->isChecked())
                    ui->filtered_tableView->scrollToBottom();
            }
    );
}

void MainWindow::play()
{
    ui->play_toolButton->setDisabled(true);
    ui->pause_toolButton->setEnabled(true);
    ui->settings_toolButton->setDisabled(true);

    //mSerial->setPortName("COM6");//mPort);         ///////// to be modified
    //mSerial->setBaudRate(115200);
    //mSerial->open(QIODevice::ReadWrite);

    mCmds->initUart->write(115200,
                           InitUart::DataSize::DATASIZE_8bits,
                           InitUart::Parity::PARITY_NONE,
                           InitUart::Stop::STOP_1bit);

    mTimer->start(500);
}

void MainWindow::pause()
{
    mTimer->stop();
    mCmds->deInitUart->write();
    ui->pause_toolButton->setDisabled(true);
    ui->play_toolButton->setEnabled(true);
    ui->settings_toolButton->setEnabled(true);
    //mSerial->close();                    ///////// to be modified
}

