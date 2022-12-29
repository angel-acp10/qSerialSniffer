#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMetaEnum>
#include <QMessageBox>
#include <QDateTime>
#include <QFontDatabase>
#include "table/Fragment.h"
#include <QDebug>
#include <memory>

#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mSettingsDialog(new SettingsDialog(this)),
      mSerial(new SerialIO(100, 3, this)), ///////// to be modified
      mTStamp0(new TimeStamp(this)),
      mTStamp1(new TimeStamp(this)),
      mCmds(new CommandManager(mSerial, mTStamp0, mTStamp1, this)),
      mFragModel(new FragmentsModel(this)),
      mTimer(new QTimer),

      mFilteredDock(new QDockWidget("Filtered Terminal", this)),
      mSearchDock(new QDockWidget("Search", this)),
      mFilteredWidget(new FilteredWidget(mFragModel, mFilteredDock)),
      mSearchWidget(new SearchWidget(mFilteredWidget, mSettingsDialog, mSearchDock))
{
    ui->setupUi(this);
    mDelegates = new Delegates(mFragModel,
                               mSettingsDialog,
                               this);

    mSerial->start(); ///////// to be modified

    initButtons();
    initEncodingList();
    initTableWidget();
    initDocks();
    initPortLabels();

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
                for(const auto &frag : lst)
                {
                    mFragModel->appendFragment(frag);
                }
            });

    connect(mTimer, &QTimer::timeout, mCmds->getAllQueue, &GetAllQueue::write);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initButtons()
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
}

void MainWindow::initEncodingList()
{
    std::unique_ptr<QList<QString>> encodingList(
                mDelegates->encoding->getEncodings() );

    ui->encoding_comboBox->addItems(*encodingList);

    connect(ui->encoding_comboBox, &QComboBox::currentTextChanged,
            mDelegates->encoding, &EncodingDelegate::showAs);
}

void MainWindow::initTableWidget()
{
    ui->tableView->setItemDelegateForColumn(0, mDelegates->time);
    ui->tableView->setItemDelegateForColumn(1, mDelegates->time);
    ui->tableView->setItemDelegateForColumn(2, mDelegates->id);
    ui->tableView->setItemDelegateForColumn(3, mDelegates->encoding);
    ui->tableView->setModel(mFragModel);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    QFont fixedFont("Monospace");
    fixedFont.setStyleHint(QFont::TypeWriter);
    ui->tableView->setFont(fixedFont);
}

void MainWindow::initDocks()
{
    mSearchDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    mSearchDock->setWidget(mSearchWidget);
    addDockWidget(Qt::RightDockWidgetArea, mSearchDock);

    mFilteredDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    mFilteredDock->setWidget(mFilteredWidget);
    addDockWidget(Qt::BottomDockWidgetArea, mFilteredDock);
}

void MainWindow::initPortLabels()
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

    mTimer->start(1000);
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

