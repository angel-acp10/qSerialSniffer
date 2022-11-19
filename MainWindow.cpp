#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMetaEnum>
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mSettingsDialog(new SettingsDialog(this)),
      mSerial(new SerialIO(this)),
      mCmds(new CommandManager(mSerial, this)),
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

    initButtons();
    initEncodingList();

    //connect(ui->settings_pushButton, &QPushButton::clicked,
            //mSettingsDialog, &SettingsDialog::exec);

    connect(ui->settings_pushButton, &QPushButton::clicked, mCmds->getId, &GetId::write);


    connect(mCmds->getId, &GetId::received,
            [this](QString id)
            {
                QMessageBox::information(this, "GetId response", id);
            });

    connect(mSerial, &SerialIO::error,
            [this]()
            {
                QMessageBox::critical(this, "SerialPort Error", "");
            });
    connect(mSerial, &SerialIO::writeTimeout,
            [this]()
            {
                QMessageBox::critical(this, "SerialPort writeTimeout", "");
            });
    connect(mSerial, &SerialIO::readTimeout,
            [this]()
            {
                QMessageBox::critical(this, "SerialPort readTimeout", "");
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initButtons()
{
    ui->pause_pushButton->setDisabled(true);
    ui->play_pushButton->setEnabled(true);

    connect(mSettingsDialog, &SettingsDialog::settingsChanged,
            this, &MainWindow::applySettings);

    connect(ui->play_pushButton, &QPushButton::clicked,
            this, &MainWindow::play);

    connect(ui->pause_pushButton, &QPushButton::clicked,
            this, &MainWindow::pause);
}

void MainWindow::initEncodingList()
{
    QList<QString> encodingList {"ASCII", "Hex", "Dec", "Bin"};
    ui->encoding_comboBox->addItems(encodingList);
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

    mSerial->setPortName("COM7");
    mSerial->setBaudRate(115200);
    mSerial->open(QIODevice::ReadWrite);
}

void MainWindow::pause()
{
    ui->pause_pushButton->setDisabled(true);
    ui->play_pushButton->setEnabled(true);
    mSerial->close();
}

