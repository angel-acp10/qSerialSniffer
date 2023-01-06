#ifndef FRAGMENTSMODEL_H
#define FRAGMENTSMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QMap>
#include <memory>
#include "Fragment.h"

class FragmentsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        kStart=0,
        kEnd,
        kId,
        kData
    };
    enum Encoding
    {
        AsciiHex=0,
        Hex,
        Dec,
        Bin
    };
    Q_ENUM(Encoding)

    enum CustomRoles {
        RawDataRole = Qt::UserRole + 1
    };

    explicit FragmentsModel(QObject *parent = nullptr);

    std::unique_ptr<QList<QString>> getEncodings() const;
    void appendFragment(Fragment &frag);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

public slots:
    void reset();
    void setEncoding(const QString& encoding);

private:
    Encoding m_encoding;
    QList<Fragment> m_fragments;
    int m_batchSize = 100;

    // Load the data for a cell
    QMap<int, QVariant> loadData(int row, int column) const;

    QString toAsciiHex(const QByteArray &arr) const;
    QString toHex(const QByteArray &arr) const;
    QString toDec(const QByteArray &arr) const;
    QString toBin(const QByteArray &arr) const;
};

#endif // FRAGMENTSMODEL_H
