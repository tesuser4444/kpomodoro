#include "pomodoro.h"

#include <QSettings>
#include <QtGlobal>

Pomodoro::Pomodoro(QObject *parent)
    : QObject(parent)
{
    QSettings s;
    m_focusMin = s.value(QStringLiteral("focus"), m_focusMin).toInt();
    m_shortMin = s.value(QStringLiteral("short"), m_shortMin).toInt();
    m_longMin = s.value(QStringLiteral("long"), m_longMin).toInt();
    m_longEvery = s.value(QStringLiteral("longEvery"), m_longEvery).toInt();
    m_autoStart = s.value(QStringLiteral("autoStart"), m_autoStart).toBool();
    m_focusMin = qBound(1, m_focusMin, 180);
    m_shortMin = qBound(1, m_shortMin, 60);
    m_longMin = qBound(1, m_longMin, 120);
    m_longEvery = qBound(2, m_longEvery, 12);
    m_total = m_remaining = phaseSeconds(Focus);

    m_timer.setInterval(200);
    m_timer.setTimerType(Qt::CoarseTimer);
    connect(&m_timer, &QTimer::timeout, this, &Pomodoro::tick);
}

int Pomodoro::phaseSeconds(Phase p) const
{
    switch (p) {
    case Focus: return m_focusMin * 60;
    case ShortBreak: return m_shortMin * 60;
    case LongBreak: return m_longMin * 60;
    }
    return 0;
}

void Pomodoro::start()
{
    if (m_running)
        return;
    m_deadline = QDeadlineTimer(qint64(m_remaining) * 1000);
    m_running = true;
    m_timer.start();
    Q_EMIT runningChanged();
}

void Pomodoro::pause()
{
    if (!m_running)
        return;
    const qint64 ms = m_deadline.remainingTime();
    if (ms <= 0) {
        finish();
        return;
    }
    m_timer.stop();
    setRemaining(int((ms + 999) / 1000));
    m_running = false;
    Q_EMIT runningChanged();
}

void Pomodoro::toggle()
{
    m_running ? pause() : start();
}

void Pomodoro::reset()
{
    const bool wasRunning = m_running;
    m_timer.stop();
    m_running = false;
    if (m_total != phaseSeconds(m_phase)) {
        m_total = phaseSeconds(m_phase);
        Q_EMIT totalChanged();
    }
    setRemaining(m_total);
    if (wasRunning)
        Q_EMIT runningChanged();
}

void Pomodoro::skip()
{
    m_timer.stop();
    const bool wasRunning = m_running;
    m_running = false;
    enter(m_phase == Focus ? nextBreak() : Focus);
    if (wasRunning)
        Q_EMIT runningChanged();
}

void Pomodoro::tick()
{
    const qint64 ms = m_deadline.remainingTime();
    if (ms <= 0) {
        finish();
        return;
    }
    setRemaining(int((ms + 999) / 1000));
}

void Pomodoro::finish()
{
    const Phase done = m_phase;
    m_timer.stop();
    m_running = false;
    if (done == Focus) {
        ++m_completed;
        Q_EMIT completedChanged();
    }
    enter(done == Focus ? nextBreak() : Focus);
    Q_EMIT runningChanged();
    Q_EMIT phaseFinished(done);
    if (m_autoStart)
        start();
}

Pomodoro::Phase Pomodoro::nextBreak() const
{
    return (m_completed > 0 && m_completed % m_longEvery == 0) ? LongBreak : ShortBreak;
}

void Pomodoro::enter(Phase p)
{
    m_phase = p;
    m_total = m_remaining = phaseSeconds(p);
    Q_EMIT phaseChanged();
    Q_EMIT totalChanged();
    Q_EMIT remainingChanged();
}

void Pomodoro::setRemaining(int s)
{
    if (s == m_remaining)
        return;
    m_remaining = s;
    Q_EMIT remainingChanged();
}

void Pomodoro::applySettings(Phase affected)
{
    save();
    Q_EMIT settingsChanged();
    // Una fase en curso o pausada a mitad conserva su duración; solo se
    // actualiza si no ha empezado todavía.
    if (affected == m_phase && !m_running && m_remaining == m_total) {
        m_total = phaseSeconds(m_phase);
        Q_EMIT totalChanged();
        setRemaining(m_total);
    }
}

void Pomodoro::save() const
{
    QSettings s;
    s.setValue(QStringLiteral("focus"), m_focusMin);
    s.setValue(QStringLiteral("short"), m_shortMin);
    s.setValue(QStringLiteral("long"), m_longMin);
    s.setValue(QStringLiteral("longEvery"), m_longEvery);
    s.setValue(QStringLiteral("autoStart"), m_autoStart);
}

void Pomodoro::setFocusMinutes(int m) { m = qBound(1, m, 180); if (m != m_focusMin) { m_focusMin = m; applySettings(Focus); } }
void Pomodoro::setShortMinutes(int m) { m = qBound(1, m, 60); if (m != m_shortMin) { m_shortMin = m; applySettings(ShortBreak); } }
void Pomodoro::setLongMinutes(int m) { m = qBound(1, m, 120); if (m != m_longMin) { m_longMin = m; applySettings(LongBreak); } }
void Pomodoro::setLongEvery(int n) { n = qBound(2, n, 12); if (n != m_longEvery) { m_longEvery = n; applySettings(Phase(-1)); } }
void Pomodoro::setAutoStart(bool on) { if (on != m_autoStart) { m_autoStart = on; applySettings(Phase(-1)); } }
