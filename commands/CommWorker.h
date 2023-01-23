#ifndef COMMWORKER_H
#define COMMWORKER_H

#include <QObject>
#include <QTimer>

#include "SerialIO.h"
#include "CommandManager.h"
#include "../TimeStamp.h"
#include "../table/FragmentsModel.h"
#include "../SettingsDialog.h"

class CommWorker : public QObject
{
    Q_OBJECT
public:
    explicit CommWorker(FragmentsModel *fragModel,
                        SettingsDialog *settings,
                        QObject *parent = nullptr);
    ~CommWorker();
    void moveChildrenToThread(QThread *thread);

public slots:
    void play();
    void pause();

private:
    SerialIO *m_ser;
    CommandManager *m_cmd;
    QTimer *m_timer;
    TimeStamp *m_tStamp;

    FragmentsModel *m_fragModel;
    SettingsDialog *m_settings;
};

#endif // COMMWORKER_H
