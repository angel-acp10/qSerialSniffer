#ifndef ENCODINGDELEGATE_H
#define ENCODINGDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>

class EncodingDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    EncodingDelegate(QWidget *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    int m_encoding;

    QString showAsAsciiHex(const QByteArray &arr) const;
    QString showAsHex(const QByteArray &arr) const;
    QString showAsDec(const QByteArray &arr) const;
    QString showAsBin(const QByteArray &arr) const;
};

#endif // ENCODINGDELEGATE_H
