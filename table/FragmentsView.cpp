#include "FragmentsView.h"

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
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
        return;

    int col = index.column();
    if(col == 0 || col == 1)
    {
        // builds the context menu
        QMenu menu(this);

        int timeDiffRows = m_timeDiff->getFragmentsCount();

        if(timeDiffRows == 1)
        {
            QAction *select = menu.addAction("1. Select for compare");
            QAction *compare = menu.addAction("2. Compare with selected");
            select->setIcon(QIcon(":/images/stopwatch.svg"));
            compare->setIcon(QIcon(":/images/stopwatch.svg"));

            // set the action to be performed
            QAction *selectedAction = menu.exec(mapToGlobal(event->pos()));
            if (selectedAction == select)
                emit selectForCompare(index);

            else if (selectedAction == compare)
                emit compareWithSelected(index);
        }
        else
        {
            QAction *select = menu.addAction("1. Select for compare");
            select->setIcon(QIcon(":/images/stopwatch.svg"));

            // set the action to be performed
            QAction *selectedAction = menu.exec(mapToGlobal(event->pos()));
            if (selectedAction == select)
                emit selectForCompare(index);
        }
    }
    else // default actions for other columns
        QTableView::contextMenuEvent(event);
}
