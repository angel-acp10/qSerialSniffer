#include "FragmentsView.h"
#include "FragmentsModel.h"
#include "qapplication.h"
#include "Delegates.h"
#include <QClipboard>

#include <qevent.h>
#include <QMenu>

FragmentsView::FragmentsView(QWidget *parent ) : QTableView(parent)
{
}

void FragmentsView::setTimeDifferenceObject(TimeDiff *const timeDiff)
{
    m_timeDiff = timeDiff;
}

void FragmentsView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex eventIndex = indexAt(event->pos());
    if (!eventIndex.isValid())
        return;

    // builds the context menu
    QMenu menu(this);
    QAction *copy = menu.addAction("Copy");
    QAction *select = menu.addAction("1. Select for compare");
    QAction *compare = menu.addAction("2. Compare with selected");
    copy->setShortcut(QKeySequence::Copy);
    copy->setIcon(QIcon(":/images/clipboard.svg"));
    select->setIcon(QIcon(":/images/stopwatch.svg"));
    compare->setIcon(QIcon(":/images/stopwatch.svg"));

    // checks which indices are selected and decide which actions are visible
    QModelIndexList selIndices = selectionModel()->selectedIndexes();
    if (selIndices.isEmpty())
        return;

    if(selIndices.length() > 1)
    {
        select->setVisible(false);
        compare->setVisible(false);
    }
    else
    {
        switch(static_cast<FragmentsModel::Column>(eventIndex.column()))
        {
        case FragmentsModel::kStart:
        case FragmentsModel::kEnd:
            if(m_timeDiff->getFragmentsCount() != 1)
                compare->setDisabled(true);
            break;

        case FragmentsModel::kNumber:
        case FragmentsModel::kId:
        case FragmentsModel::kData:
            select->setVisible(false);
            compare->setVisible(false);
            break;
        }
    }

    // set the action to be performed
    QAction *selectedAction = menu.exec(mapToGlobal(event->pos()));
    if(selectedAction == copy)
        handleCopyAction();

    else if (selectedAction == select)
        emit selectForCompare(eventIndex);

    else if (selectedAction == compare)
        emit compareWithSelected(eventIndex);

}

void FragmentsView::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy))
    {
        handleCopyAction();
        return;
    }
}

void FragmentsView::handleCopyAction()
{
    // Get the selected indices
    QModelIndexList indices = selectionModel()->selectedIndexes();
    if (indices.isEmpty())
    {
        return;
    }

    // Sort the indices by row and column
    std::sort(indices.begin(), indices.end());

    // Create a QStringList to hold the formatted data
    QStringList rows;

    // Iterate over the indices and append the formatted data to the QStringList
    int lastRow = -1;
    for (const QModelIndex &index : indices)
    {
        if (index.row() != lastRow)
        {
            lastRow = index.row();
            rows.append(QString());
        }

        QAbstractItemDelegate *delegate = itemDelegateForIndex(index);
        TimeDelegate *timeDelegate;
        IdDelegate *idDelegate;
        EncodingDelegate *encDelegate;
        QString text;

        switch( static_cast<FragmentsModel::Column>(index.column()) )
        {
        case FragmentsModel::kNumber:
            text = index.data().toString();
            break;
        case FragmentsModel::kStart:
        case FragmentsModel::kEnd:
            timeDelegate = qobject_cast<TimeDelegate*>(delegate);
            Q_ASSERT(timeDelegate);
            text = timeDelegate->displayText(index.data(), QLocale());
            break;

        case FragmentsModel::kId:
            idDelegate = qobject_cast<IdDelegate*>(delegate);
            Q_ASSERT(idDelegate);
            text = idDelegate->displayText(index.data(), QLocale());
            break;

        case FragmentsModel::kData:
            encDelegate = qobject_cast<EncodingDelegate*>(delegate);
            Q_ASSERT(encDelegate);
            text = encDelegate->displayText(index.data(), QLocale());
            break;
        }
        rows.last().append(text+'\t');
    }

    // Copy the formatted data to the clipboard
    QApplication::clipboard()->setText(rows.join('\n'));
}

