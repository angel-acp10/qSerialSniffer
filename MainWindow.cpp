#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMetaEnum>
#include <QMessageBox>
#include <QDateTime>
#include <QFontDatabase>
#include "table/Fragment.h"

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
    ui->pause_pushButton->setDisabled(true);
    ui->play_pushButton->setEnabled(true);
    ui->settings_pushButton->setEnabled(true);

    connect(ui->settings_pushButton, &QPushButton::clicked,
                mSettingsDialog, &SettingsDialog::exec);

    connect(ui->play_pushButton, &QPushButton::clicked, this, &MainWindow::play);
    connect(ui->play_pushButton, &QPushButton::clicked, mTStamp0, &TimeStamp::onPlayClicked);
    connect(ui->play_pushButton, &QPushButton::clicked, mTStamp1, &TimeStamp::onPlayClicked);

    connect(ui->pause_pushButton, &QPushButton::clicked, this, &MainWindow::pause);
    connect(ui->pause_pushButton, &QPushButton::clicked, mTStamp0, &TimeStamp::onPauseClicked);
    connect(ui->pause_pushButton, &QPushButton::clicked, mTStamp1, &TimeStamp::onPauseClicked);
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
    ui->received_tableView->setItemDelegateForColumn(0, mTimeDelegate);
    ui->received_tableView->setItemDelegateForColumn(1, mTimeDelegate);
    ui->received_tableView->setItemDelegateForColumn(2, mIdDelegate);
    ui->received_tableView->setItemDelegateForColumn(3, mEncDelegate);
    ui->received_tableView->setModel(mFragModel);
    ui->received_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //ui->received_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->received_tableView->horizontalHeader()->setStretchLastSection(true);

    //const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    QFont fixedFont("Monospace");
    fixedFont.setStyleHint(QFont::TypeWriter);
    ui->received_tableView->setFont(fixedFont);
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
    ui->play_pushButton->setDisabled(true);
    ui->pause_pushButton->setEnabled(true);
    ui->settings_pushButton->setDisabled(true);

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
    ui->pause_pushButton->setDisabled(true);
    ui->play_pushButton->setEnabled(true);
    ui->settings_pushButton->setEnabled(true);
    //mSerial->close();                    ///////// to be modified
}

