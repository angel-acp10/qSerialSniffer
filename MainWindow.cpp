#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMetaEnum>
#include <QMessageBox>
#include <QDateTime>
#include "model/Fragment.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mSettingsDialog(new SettingsDialog(this)),
      mSerial(new SerialIO(100, 3, this)), ///////// to be modified
      mTStamp0(new TimeStamp(this)),
      mTStamp1(new TimeStamp(this)),
      mCmds(new CommandManager(mSerial, mTStamp0, mTStamp1, this)),
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
                    ui->received_tableWidget->insertRow( ui->received_tableWidget->rowCount() );
                    ui->received_tableWidget->setItem( ui->received_tableWidget->rowCount()-1,
                                                       0, new QTableWidgetItem( QString("%1").arg(frag.getStartAcumUs()) ));
                    ui->received_tableWidget->setItem( ui->received_tableWidget->rowCount()-1,
                                                       1, new QTableWidgetItem( QString("%1").arg(frag.getEndAcumUs()) ));
                    ui->received_tableWidget->setItem( ui->received_tableWidget->rowCount()-1,
                                                       2, new QTableWidgetItem( QString("%1").arg((int)frag.getPort()) ));
                    ui->received_tableWidget->setItem( ui->received_tableWidget->rowCount()-1,
                                                       3, new QTableWidgetItem( QString(frag.getData()) ));
                }
            });


    //connect(ui->test_pushButton, &QPushButton::clicked, mCmds->getAllQueue, &GetAllQueue::write);

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
    ui->received_tableWidget->setColumnCount(4);
    ui->received_tableWidget->setRowCount(0);
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
    //mSerial->close();                    ///////// to be modified
}

