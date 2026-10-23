#pragma once

#include <QDeadlineTimer>
#include <QObject>
#include <QTimer>

class Pomodoro : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Phase phase READ phase NOTIFY phaseChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(int remaining READ remaining NOTIFY remainingChanged)
    Q_PROPERTY(int total READ total NOTIFY totalChanged)
    Q_PROPERTY(int completed READ completed NOTIFY completedChanged)
    Q_PROPERTY(int focusMinutes READ focusMinutes WRITE setFocusMinutes NOTIFY settingsChanged)
    Q_PROPERTY(int shortMinutes READ shortMinutes WRITE setShortMinutes NOTIFY settingsChanged)
    Q_PROPERTY(int longMinutes READ longMinutes WRITE setLongMinutes NOTIFY settingsChanged)
    Q_PROPERTY(int longEvery READ longEvery WRITE setLongEvery NOTIFY settingsChanged)
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart NOTIFY settingsChanged)

public:
    enum Phase { Focus, ShortBreak, LongBreak };
    Q_ENUM(Phase)

    explicit Pomodoro(QObject *parent = nullptr);

    Phase phase() const { return m_phase; }
    bool running() const { return m_running; }
    int remaining() const { return m_remaining; }
    int total() const { return m_total; }
    int completed() const { return m_completed; }
    int focusMinutes() const { return m_focusMin; }
    int shortMinutes() const { return m_shortMin; }
    int longMinutes() const { return m_longMin; }
    int longEvery() const { return m_longEvery; }
    bool autoStart() const { return m_autoStart; }

    void setFocusMinutes(int m);
    void setShortMinutes(int m);
    void setLongMinutes(int m);
    void setLongEvery(int n);
    void setAutoStart(bool on);

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void toggle();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void skip();

Q_SIGNALS:
    void phaseChanged();
    void totalChanged();
    void runningChanged();
    void remainingChanged();
    void completedChanged();
    void settingsChanged();
    void phaseFinished(Pomodoro::Phase finished);

private:
    int phaseSeconds(Phase p) const;
    void tick();
    void finish();
    void enter(Phase p);
    void setRemaining(int s);
    void applySettings(Phase affected);
    Phase nextBreak() const;
    void save() const;

    QTimer m_timer;
    QDeadlineTimer m_deadline;
    Phase m_phase = Focus;
    bool m_running = false;
    int m_remaining = 0;
    int m_total = 0;
    int m_completed = 0;
    int m_focusMin = 25;
    int m_shortMin = 5;
    int m_longMin = 15;
    int m_longEvery = 4;
    bool m_autoStart = false;
};
