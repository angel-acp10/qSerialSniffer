#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include <QSerialPortInfo>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    fillBauds();
    fillParity();
    fillDataSize();
    scanPorts();
    fillColors();

    connect(ui->scan_pushButton,    &QPushButton::clicked,
            this,                   &SettingsDialog::scanPorts);
    connect(ui->ok_pushButton,      &QPushButton::clicked,
            this,                   &SettingsDialog::validateData);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

const QString& SettingsDialog::getPort() const
{
    return m_port;
}
int SettingsDialog::getBauds() const
{
    return m_bauds;
}
const QString& SettingsDialog::getParity() const
{
    return m_parity;
}
int SettingsDialog::getDataSize() const
{
    return m_dataSize;
}
const QColor& SettingsDialog::getColorA() const
{
    return m_colorA;
}
const QColor& SettingsDialog::getColorB() const
{
    return m_colorB;
}
const QString& SettingsDialog::getAliasA() const
{
    return m_aliasA;
}
const QString& SettingsDialog::getAliasB() const
{
    return m_aliasB;
}

void SettingsDialog::fillBauds()
{
    QList<QString> baudList {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    ui->baudrate_comboBox->addItems(baudList);
}

void SettingsDialog::fillParity()
{
    QList<QString> parityList {"None", "Even", "Odd"};
    ui->parity_comboBox->addItems(parityList);
}

void SettingsDialog::fillDataSize()
{
    QList<QString> dataSizeList {"8", "9"};
    ui->dataSize_comboBox->addItems(dataSizeList);
}

void SettingsDialog::scanPorts()
{
    ui->port_comboBox->clear();
    for(const auto &port : QSerialPortInfo::availablePorts())
        ui->port_comboBox->addItem(port.portName());
}

void SettingsDialog::fillColors()
{
    QList<QString> colorList {"black", "red", "orange", "blue", "green"};
    ui->colorA_comboBox->addItems(colorList);
    ui->colorB_comboBox->addItems(colorList);
}

void SettingsDialog::validateData()
{
    bool resBauds = true;
    bool resDataSize = true;

    QString port = ui->port_comboBox->currentText();
    int bauds = ui->baudrate_comboBox->currentText().toInt(&resBauds);
    QString parity = ui->parity_comboBox->currentText();
    int dataSize = ui->dataSize_comboBox->currentText().toInt(&resDataSize);
    QColor colorA =  QColor(ui->colorA_comboBox->currentText());
    QColor colorB =  QColor(ui->colorB_comboBox->currentText());
    QString aliasA = ui->aliasA_lineEdit->text();
    QString aliasB = ui->aliasB_lineEdit->text();

    if(!resBauds) // bauds: error on string to int conversion
    {
        QMessageBox::critical(this, tr("Error"), tr("Invalid baudrate."));
        return;
    }
    if(!resDataSize) // bauds: error on string to int conversion
    {
        QMessageBox::critical(this, tr("Error"), tr("Invalid data size."));
        return;
    }
    if(!colorA.isValid())
    {
        QMessageBox::critical(this, tr("Error"), tr("Color A is not valid."));
        return;
    }
    if(!colorB.isValid())
    {
        QMessageBox::critical(this, tr("Error"), tr("Color B is not valid."));
        return;
    }
    if(colorA == colorB)
    {
        QMessageBox::warning(this, tr("Error"), tr("Both colors are the same."));
    }
    if(aliasA == aliasB)
    {
        QMessageBox::warning(this, tr("Error"), tr("Both aliases are the same."));
    }

    // amit signals
    if(port != m_port)
    {
        m_port = port;
        emit portChanged(m_port);
    }
    if(bauds != m_bauds)
    {
        m_bauds = bauds;
        emit baudsChanged(m_bauds);
    }
    if(parity != m_parity)
    {
        m_parity = parity;
        emit parityChanged(m_parity);
    }
    if(dataSize != m_dataSize)
    {
        m_dataSize = dataSize;
        emit dataSizeChanged(m_dataSize);
    }
    if(colorA != m_colorA)
    {
        m_colorA = colorA;
        emit colorAChanged(m_colorA);
    }
    if(colorB != m_colorB)
    {
        m_colorB = colorB;
        emit colorBChanged(m_colorB);
    }
    if(aliasA != m_aliasA)
    {
        m_aliasA = aliasA;
        emit aliasAChanged(m_aliasA);
    }
    if(aliasB != m_aliasB)
    {
        m_aliasB = aliasB;
        emit aliasBChanged(m_aliasB);
    }

    accept();
}
