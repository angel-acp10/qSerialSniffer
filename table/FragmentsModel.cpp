#include "FragmentsModel.h"
#include "qmetaobject.h"

FragmentsModel::FragmentsModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

std::unique_ptr<QList<QString>> FragmentsModel::getEncodings() const
{
    std::unique_ptr<QList<QString>> list(new QList<QString>);
    const QMetaObject& mo = FragmentsModel::staticMetaObject;
    int enumIndex = mo.indexOfEnumerator("Encoding");
    QMetaEnum metaEnum = mo.enumerator(enumIndex);
    for (int i = 0; i < metaEnum.keyCount(); ++i)
        list->append(QString::fromLatin1(metaEnum.key(i)));
    return list;
}

void FragmentsModel::appendFragment(Fragment &frag)
{
    int rowIndex = rowCount();
    frag.setAsciiHex(toAsciiHex(frag.getData()));
    frag.setHex(toHex(frag.getData()));
    frag.setDec(toDec(frag.getData()));
    frag.setBin(toBin(frag.getData()));
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
    if(!index.isValid())
        return QVariant();

    int row = index.row();
    if(role == Qt::DisplayRole)
    {
        switch(static_cast<Column>(index.column()))
        {
        case Column::kStart:    return m_fragments[row].getStartAcumUs();
        case Column::kEnd:      return m_fragments[row].getEndAcumUs();
        case Column::kId:       return m_fragments[row].getPort();
        case Column::kData:     
            switch(m_encoding)
            {
            case Encoding::Hex:         return m_fragments[row].getHex();
            case Encoding::Dec:         return m_fragments[row].getDec();
            case Encoding::Bin:         return m_fragments[row].getBin();
            default:
            case Encoding::AsciiHex:    return m_fragments[row].getAsciiHex();
            }
        default:    
            return QVariant();
        }
    }
    else if(role == RawDataRole)
    {
        switch(static_cast<Column>(index.column()))
        {
        case Column::kStart:    return m_fragments[row].getStartAcumUs();
        case Column::kEnd:      return m_fragments[row].getEndAcumUs();
        case Column::kId:       return m_fragments[row].getPort();
        case Column::kData:     return m_fragments[row].getData();
        default:    
            return QVariant();
        }
    }
    return QVariant();
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

bool FragmentsModel::canFetchMore(const QModelIndex& parent) const
{
    return parent.isValid() ? false : rowCount() < m_fragments.size();
}

void FragmentsModel::fetchMore(const QModelIndex& parent)
{
    if (!canFetchMore(parent))
        return;

    int rowsToFetch = qMin(m_batchSize, m_fragments.size() - rowCount());
    int firstRow = rowCount();

    beginInsertRows(QModelIndex(), firstRow, firstRow + rowsToFetch - 1);

    // Load the data for the new rows
    for (int row = firstRow; row < firstRow + rowsToFetch; row++)
    {
        for (int column = 0; column < columnCount(); column++)
        {
            // Load the data for the cell and insert it into the model
            QModelIndex index = createIndex(row, column);
            QMap<int, QVariant> data = loadData(row, column);
            setItemData(index, data);
            setData(index, data[Qt::DisplayRole]);
        }
    }

    endInsertRows();
}

void FragmentsModel::reset()
{
    beginResetModel();
    m_fragments.clear();
    endResetModel();
}

void FragmentsModel::setEncoding(const QString &encoding)
{
    const QMetaObject& mo = FragmentsModel::staticMetaObject;
    int enumIndex = mo.indexOfEnumerator("Encoding");
    QMetaEnum metaEnum = mo.enumerator(enumIndex);
    int value = metaEnum.keyToValue(encoding.toLatin1());

    if (value == -1)
        m_encoding = Encoding::AsciiHex;
    else
        m_encoding = static_cast<Encoding>(value);

    int rows = rowCount();
    QModelIndex topL = index(0, Column::kData);
    QModelIndex bottomR = index(rows, Column::kData);
    emit dataChanged(topL, bottomR);
}

QMap<int, QVariant> FragmentsModel::loadData(int row, int column) const
{
    QMap<int, QVariant> data;

    // Load the data for the cell and return it
    const Fragment& frag = m_fragments[row];
    switch (static_cast<Column>(column))
    {
    case Column::kStart:
        data[Qt::DisplayRole] = frag.getStartAcumUs();
        break;
    case Column::kEnd:
        data[Qt::DisplayRole] = frag.getEndAcumUs();
        break;
    case Column::kId:
        data[Qt::DisplayRole] = frag.getNumber();
        break;
    case Column::kData:
        switch (m_encoding)
        {
        case Encoding::AsciiHex:
            data[Qt::DisplayRole] = frag.getAsciiHex();
            break;
        case Encoding::Hex:
            data[Qt::DisplayRole] = frag.getHex();
            break;
        case Encoding::Dec:
            data[Qt::DisplayRole] = frag.getDec();
            break;
        case Encoding::Bin:
            data[Qt::DisplayRole] = frag.getBin();
            break;
        }
        data[CustomRoles::RawDataRole] = frag.getData();
        break;
    }

    return data;
}

QString FragmentsModel::toAsciiHex(const QByteArray &arr) const
{
    int bytes = arr.size();
    QString fullStr = QString(4*bytes, ' '); // 1 byte represented as "ascii+hex" might need up to 4 chars
    int idxStr = 0;

    QString s;

    for(int i=0; i<bytes; i++)
    {
        if(arr[i]<0x20 || arr[i]>0x7E) // not printable
        {
            s = QString("{%1}").arg((quint8)arr[i], 2, 16, QLatin1Char('0')).toUpper();
            fullStr.replace(idxStr, 4, s);
            idxStr += 4;
        }
        else
        {
            s = QString(arr[i]);
            fullStr.replace(idxStr, 1, s);
            idxStr++;
        }
    }
    return fullStr.mid(0, idxStr);
}

QString FragmentsModel::toHex(const QByteArray &arr) const
{
    return QString(arr.toHex(' ')).toUpper();
}
QString FragmentsModel::toDec(const QByteArray &arr) const
{
    int bytes = arr.size();
    QString fullStr = QString(4*bytes, ' '); // 1 byte represented as dec needs 3 chars + 1 separator(space)
    QString s;

    for(int i=0; i<bytes; i++)
    {
        s = QString("%1").arg((quint8)arr[i], 3, 10, QLatin1Char('0'));
        fullStr.replace(4*i, 3, s);
    }
    return fullStr;
}
QString FragmentsModel::toBin(const QByteArray &arr) const
{
    int bytes = arr.size();
    QString fullStr = QString(9*bytes, ' '); // 1 byte represented as bin needs 8 chars + 1 separator(space)
    QString s;

    for(int i=0; i<bytes; i++)
    {
        s = QString("%1").arg((quint8)arr[i], 8, 2, QLatin1Char('0'));
        fullStr.replace(9*i, 8, s);
    }
    return fullStr;
}
