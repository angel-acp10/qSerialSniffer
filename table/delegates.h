#ifndef DELEGATES_H
#define DELEGATES_H

#include <QStyledItemDelegate>
#include <QObject>
#include <QColor>
#include <QPainter>
#include <QAbstractTableModel>
#include <QAbstractProxyModel>
#include <QMetaEnum>
#include <memory>
#include "../SettingsDialog.h"

class TimeDelegate;
class IdDelegate;
class EncodingDelegate;

//------------------------
// Delegates
//------------------------
class Delegates : public QObject
{
    Q_OBJECT
public:
    explicit Delegates(QAbstractTableModel *model,
                       QAbstractProxyModel *filteredModel,
                       SettingsDialog *settings,
                       QObject *parent = nullptr);

    TimeDelegate *time;
    IdDelegate *id;
    EncodingDelegate *encoding;
};

//------------------------
// TimeDelegate
//------------------------
class TimeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TimeDelegate(QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    const quint64 US_PER_SEC = 1000000;
    const quint64 SEC_PER_MIN = 60;
    const quint64 MIN_PER_HOUR = 60;
};

//------------------------
// IdDelegate
//------------------------
class IdDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit IdDelegate(SettingsDialog *settings, QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    SettingsDialog *m_settings;
};

//------------------------
// EncodingDelegate
//------------------------
class EncodingDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum Encoding{
        AsciiHex=0,
        Hex,
        Dec,
        Bin
    };
    Q_ENUM(Encoding)
    EncodingDelegate(QAbstractTableModel *model,
                     QAbstractProxyModel *filteredModel,
                     QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const override;
    std::unique_ptr<QList<QString>> getEncodings() const;

public slots:
    void showAs(const QString& encoding);

private:
    Encoding m_encoding;
    QAbstractTableModel* m_model;
    QAbstractProxyModel* m_filteredModel;

    QString showAsAsciiHex(const QByteArray &arr) const;
    QString showAsHex(const QByteArray &arr) const;
    QString showAsDec(const QByteArray &arr) const;
    QString showAsBin(const QByteArray &arr) const;
};

#endif // DELEGATES_H
