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
    (void)parent;
    return m_fragments.size();
}

int FragmentsModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return 4; // start, end, id, data
}

QVariant FragmentsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    int row = index.row();
    switch(static_cast<Column>(index.column()))
    {
    case Column::kStart:    return m_fragments[row].getStartAcumUs();
    case Column::kEnd:      return m_fragments[row].getEndAcumUs();
    case Column::kId:       return m_fragments[row].getPort();
    case Column::kData:     return m_fragments[row].getData();
    default:    return QVariant();
    }
}

QVariant FragmentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
    {
        switch(static_cast<Column>(section))
        {
        case Column::kStart:    return QString("Start");
        case Column::kEnd:      return QString("End");
        case Column::kId:       return QString("ID");
        case Column::kData:     return QString("Data");
        default:    return QVariant();
        }
    }
    else if(orientation == Qt::Vertical)
        return m_fragments[section].getNumber();

    else
        return QVariant();
}

void FragmentsModel::reset()
{
    beginResetModel();
    m_fragments.clear();
    endResetModel();
}
