#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMetaEnum>
#include <QMessageBox>
#include <QDateTime>
#include <QFontDatabase>
#include "table/Fragment.h"
#include <QDebug>

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
      mTimeDelegate(new TimeDelegate(this)),
      mIdDelegate(new IdDelegate(this)),
      mEncDelegate(new EncodingDelegate(this)),
      mTimer(new QTimer),

      mExpressionTree(new ExpressionTree()),
      mFilteredDock(new QDockWidget("Filtered Terminal", this)),
      mSearchDock(new QDockWidget("Search", this)),
      mFilteredWidget(new FilteredWidget(mFragModel, mFilteredDock)),
      mSearchWidget(new SearchWidget(mFilteredWidget, mSearchDock)),

      mPort(""),
      mSnifferBaudrate(9600),
      mSnifferParity("None"),
      mSnifferDataSize(8),
      mAliasA(QString("portA")),
      mAliasB(QString("portB")),
      mColorA(QColor("black")),
      mColorB(QColor("black"))
{
    ui->setupUi(this);

    mSerial->start(); ///////// to be modified

    initButtons();
    initEncodingList();
    initTableWidget();
    initDocks();

    connect(mSettingsDialog, &SettingsDialog::settingsChanged,
            this, &MainWindow::applySettings);


    connect(mCmds->getId, &GetId::received,
            [this](GetId::Status, QString id)
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
            [this](QList<Fragment> lst)
            {
                for(auto frag : lst)
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
}

void MainWindow::initEncodingList()
{
    QList<QString> encodingList {"ASCII", "Hex", "Dec", "Bin"};
    ui->encoding_comboBox->addItems(encodingList);
}

void MainWindow::initTableWidget()
{
    mIdDelegate->setId2("PC");
    mIdDelegate->setId3("MCU");
    ui->tableView->setItemDelegateForColumn(0, mTimeDelegate);
    ui->tableView->setItemDelegateForColumn(1, mTimeDelegate);
    ui->tableView->setItemDelegateForColumn(2, mIdDelegate);
    ui->tableView->setItemDelegateForColumn(3, mEncDelegate);
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

void MainWindow::applySettings(const QString &port,
                               const int bauds, const QString &parity, const int dataSize,
                               const QColor &colorA, const QColor &colorB,
                               const QString &aliasA, const QString &aliasB)
{
    QString labelA = QString("Port A (%1) @ %2 bauds").arg(aliasA).arg(bauds);
    QString labelB = QString("Port B (%1) @ %2 bauds").arg(aliasB).arg(bauds);
    QString styleSheetA = QString("QLabel {color : %1}").arg(colorA.name());
    QString styleSheetB = QString("QLabel {color : %1}").arg(colorB.name());

    mPort = port;

    ui->portA_label->setText(labelA);
    ui->portB_label->setText(labelB);
    ui->portA_label->setStyleSheet(styleSheetA);
    ui->portB_label->setStyleSheet(styleSheetB);

    mSnifferBaudrate = bauds;
    mSnifferParity = parity;
    mSnifferDataSize = dataSize;
    mAliasA = aliasA;
    mAliasB = aliasB;
    mColorA = colorA;
    mColorB = colorB;
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

