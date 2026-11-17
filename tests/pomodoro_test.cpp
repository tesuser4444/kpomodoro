#include "pomodoro.h"

#include <QCoreApplication>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

class PomodoroTest : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_settingsDir;

private Q_SLOTS:
    void initTestCase()
    {
        QVERIFY(m_settingsDir.isValid());
        QCoreApplication::setOrganizationName(QStringLiteral("kpomodoro-tests"));
        QCoreApplication::setApplicationName(QStringLiteral("kpomodoro-tests"));
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_settingsDir.path());
    }

    void init()
    {
        QSettings settings;
        settings.clear();
        settings.sync();
    }

    void startsWithFocusDefaults()
    {
        Pomodoro pomodoro;

        QCOMPARE(pomodoro.phase(), Pomodoro::Focus);
        QCOMPARE(pomodoro.total(), 25 * 60);
        QCOMPARE(pomodoro.remaining(), pomodoro.total());
        QCOMPARE(pomodoro.completed(), 0);
        QVERIFY(!pomodoro.running());
    }

    void clampsSettingsAndUpdatesUntouchedPhase()
    {
        Pomodoro pomodoro;
        QSignalSpy settingsSpy(&pomodoro, &Pomodoro::settingsChanged);
        QSignalSpy totalSpy(&pomodoro, &Pomodoro::totalChanged);

        pomodoro.setFocusMinutes(0);
        QCOMPARE(pomodoro.focusMinutes(), 1);
        QCOMPARE(pomodoro.total(), 60);
        QCOMPARE(pomodoro.remaining(), 60);

        pomodoro.setFocusMinutes(181);
        QCOMPARE(pomodoro.focusMinutes(), 180);
        QCOMPARE(pomodoro.total(), 180 * 60);
        QCOMPARE(pomodoro.shortMinutes(), 5);

        pomodoro.setShortMinutes(0);
        QCOMPARE(pomodoro.shortMinutes(), 1);
        pomodoro.setLongMinutes(121);
        QCOMPARE(pomodoro.longMinutes(), 120);
        pomodoro.setLongEvery(1);
        QCOMPARE(pomodoro.longEvery(), 2);

        QCOMPARE(settingsSpy.count(), 5);
        QCOMPARE(totalSpy.count(), 2);
    }

    void skipMovesBetweenFocusAndBreak()
    {
        Pomodoro pomodoro;
        QSignalSpy phaseSpy(&pomodoro, &Pomodoro::phaseChanged);

        pomodoro.skip();
        QCOMPARE(pomodoro.phase(), Pomodoro::ShortBreak);
        QCOMPARE(pomodoro.total(), 5 * 60);
        QCOMPARE(pomodoro.remaining(), pomodoro.total());

        pomodoro.skip();
        QCOMPARE(pomodoro.phase(), Pomodoro::Focus);
        QCOMPARE(pomodoro.total(), 25 * 60);
        QCOMPARE(pomodoro.completed(), 0);
        QCOMPARE(phaseSpy.count(), 2);
    }

    void resetStopsTimerAndUsesLatestDuration()
    {
        Pomodoro pomodoro;
        QSignalSpy runningSpy(&pomodoro, &Pomodoro::runningChanged);

        pomodoro.setFocusMinutes(30);
        pomodoro.start();
        QVERIFY(pomodoro.running());

        pomodoro.setFocusMinutes(40);
        QCOMPARE(pomodoro.total(), 30 * 60);

        pomodoro.reset();
        QVERIFY(!pomodoro.running());
        QCOMPARE(pomodoro.total(), 40 * 60);
        QCOMPARE(pomodoro.remaining(), pomodoro.total());
        QCOMPARE(runningSpy.count(), 2);
    }

    void settingsPersistAcrossInstances()
    {
        {
            Pomodoro pomodoro;
            pomodoro.setFocusMinutes(35);
            pomodoro.setLongEvery(3);
            pomodoro.setAutoStart(true);
        }

        Pomodoro restored;
        QCOMPARE(restored.focusMinutes(), 35);
        QCOMPARE(restored.longEvery(), 3);
        QVERIFY(restored.autoStart());
        QCOMPARE(restored.total(), 35 * 60);
    }

    void clampsStoredSettingsOnLoad()
    {
        QSettings settings;
        settings.setValue(QStringLiteral("focus"), 0);
        settings.setValue(QStringLiteral("short"), 100);
        settings.setValue(QStringLiteral("long"), -1);
        settings.setValue(QStringLiteral("longEvery"), 99);
        settings.sync();

        Pomodoro pomodoro;

        QCOMPARE(pomodoro.focusMinutes(), 1);
        QCOMPARE(pomodoro.shortMinutes(), 60);
        QCOMPARE(pomodoro.longMinutes(), 1);
        QCOMPARE(pomodoro.longEvery(), 12);
        QCOMPARE(pomodoro.total(), 60);
    }
};

QTEST_GUILESS_MAIN(PomodoroTest)

#include "pomodoro_test.moc"
