#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <QObject>

class TimeStamp : public QObject
{
    Q_OBJECT

public:
    explicit TimeStamp(QObject *parent = nullptr);
    ~TimeStamp();

    quint64 rawToUs(const quint32 ts_raw) const;
    quint64 deltaUs(const quint64 prevTs_us, const quint64 ts_us) const;
    quint64 computeAccumUs_fromUs(const quint64 ts_us);
    quint64 computeAccumUs_fromRaw(const quint32 ts_raw);
    void resetAccumUs();

public slots:
    void onPauseClicked();
    void onPlayClicked();

private:
    enum Stage{
        STAGE_RESET=0,
        STAGE_PAUSE,
        STAGE_PLAY
    };
    Stage m_stage;

    quint64 m_accumUs;
    quint64 m_prevTsUs;

    quint64 m_onPauseTsUnix; // unix time in ms
};

#endif // TIMESTAMP_H
