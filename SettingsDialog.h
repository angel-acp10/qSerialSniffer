#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    void loadSettings(const QString &file);

    const QString& getPort() const;
    int getBauds() const;
    const QString& getParity() const;
    int getDataSize() const;
    const QColor& getColorA() const;
    const QColor& getColorB() const;
    const QString& getAliasA() const;
    const QString& getAliasB() const;

signals:
    void portChanged(const QString& port);
    void baudsChanged(const int bauds);
    void parityChanged(const QString& parity);
    void dataSizeChanged(const int dataSize);
    void colorAChanged(const QColor& colorA);
    void colorBChanged(const QColor& colorB);
    void aliasAChanged(const QString& aliasA);
    void aliasBChanged(const QString& aliasB);

// methods
private slots:
    void fillBauds();
    void fillParity();
    void fillDataSize();
    void scanPorts();
    void fillColors();
    void validateData();

private:
    Ui::SettingsDialog *ui;

    QString m_file;

    QString m_port;
    int m_bauds;
    QString m_parity;
    int m_dataSize;
    QColor m_colorA;
    QColor m_colorB;
    QString m_aliasA;
    QString m_aliasB;
};

#endif // SETTINGSDIALOG_H
