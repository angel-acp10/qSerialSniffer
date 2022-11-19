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

// methods
private slots:
    void fillBauds();
    void fillParity();
    void fillDataSize();
    void scanPorts();
    void fillColors();
    void validateData();

signals:
    void settingsChanged(const QString &port,
                         const int bauds, const QString &parity, const int dataSize,
                         const QColor &colorA, const QColor &colorB,
                         const QString &aliasA, const QString &aliasB);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
