#ifndef FRAGMENTSVIEW_H
#define FRAGMENTSVIEW_H

#include <QTableView>
#include <QObject>
#include "TimeDiff.h"

class FragmentsView : public QTableView
{
    Q_OBJECT
public:
    FragmentsView(QWidget *parent = nullptr);
    void setTimeDifferenceObject(TimeDiff *const timeDiff);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    TimeDiff *m_timeDiff;

    void handleCopyAction();

signals:
    void selectForCompare(const QModelIndex &index);
    void compareWithSelected(const QModelIndex &index);
};

#endif // FRAGMENTSVIEW_H
