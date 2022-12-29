#ifndef FRAGMENTSMODEL_H
#define FRAGMENTSMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include "Fragment.h"

class FragmentsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit FragmentsModel(QObject *parent = nullptr);

    void appendFragment(const Fragment &frag);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    void reset();

private:
    QList<Fragment> m_fragments;
};

#endif // FRAGMENTSMODEL_H
