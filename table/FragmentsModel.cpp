#include "FragmentsModel.h"

FragmentsModel::FragmentsModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

void FragmentsModel::appendFragment(const Fragment &frag)
{
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    m_fragments.append(frag);
    endInsertRows();
}

int FragmentsModel::rowCount(const QModelIndex &parent) const
{
    return m_fragments.size();
}

int FragmentsModel::columnCount(const QModelIndex &parent) const
{
    return 4; // uartId, startTStamp, endTStamp, data
}

QVariant FragmentsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    switch(index.column())
    {
    case 0:     return m_fragments[index.row()].getStartAcumUs();
    case 1:     return m_fragments[index.row()].getEndAcumUs();
    case 2:     return m_fragments[index.row()].getPort();
    case 3:     return m_fragments[index.row()].getData();
    default:    return QVariant();
    }
}

QVariant FragmentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}
