#include "pomodoro.h"

#include <KNotification>
#include <KStatusNotifierItem>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>

static QString clock(int secs)
{
    return QStringLiteral("%1:%2").arg(secs / 60, 2, 10, QLatin1Char('0')).arg(secs % 60, 2, 10, QLatin1Char('0'));
}

static QString phaseName(Pomodoro::Phase p)
{
    switch (p) {
    case Pomodoro::Focus: return QStringLiteral("Enfoque");
    case Pomodoro::ShortBreak: return QStringLiteral("Descanso corto");
    case Pomodoro::LongBreak: return QStringLiteral("Descanso largo");
    }
    return {};
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("kpomodoro"));
    app.setApplicationName(QStringLiteral("kpomodoro"));
    app.setApplicationVersion(QStringLiteral(KPOMODORO_VERSION));
    app.setDesktopFileName(QStringLiteral("kpomodoro"));
    app.setQuitOnLastWindowClosed(false);

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE"))
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));

    Pomodoro pomodoro;

    QQmlApplicationEngine engine;
    qmlRegisterSingletonInstance("org.kpomodoro.app", 1, 0, "Pomodoro", &pomodoro);
    engine.loadFromModule("org.kpomodoro.app", "Main");
    if (engine.rootObjects().isEmpty()) {
        qCritical("No se pudo cargar la interfaz. ¿Están instalados kirigami y qqc2-desktop-style?");
        return 1;
    }

    auto *tray = new KStatusNotifierItem(QStringLiteral("kpomodoro"), &app);
    tray->setCategory(KStatusNotifierItem::ApplicationStatus);
    tray->setIconByName(QStringLiteral("chronometer"));
    tray->setTitle(QStringLiteral("Pomodoro"));
    tray->setAssociatedWindow(qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst()));

    auto updateTray = [&] {
        tray->setToolTip(QStringLiteral("chronometer"),
                         QStringLiteral("%1 · %2").arg(phaseName(pomodoro.phase()), clock(pomodoro.remaining())),
                         QStringLiteral("%1 pomodoros completados").arg(pomodoro.completed()));
        tray->setStatus(pomodoro.running() ? KStatusNotifierItem::Active : KStatusNotifierItem::Passive);
    };
    QObject::connect(&pomodoro, &Pomodoro::remainingChanged, tray, updateTray);
    QObject::connect(&pomodoro, &Pomodoro::runningChanged, tray, updateTray);
    QObject::connect(&pomodoro, &Pomodoro::phaseChanged, tray, updateTray);
    updateTray();

    QObject::connect(&pomodoro, &Pomodoro::phaseFinished, &app, [&pomodoro](Pomodoro::Phase done) {
        QString title, text;
        if (done == Pomodoro::Focus) {
            const bool isLong = pomodoro.phase() == Pomodoro::LongBreak;
            title = QStringLiteral("¡Deja de trabajar! Pomodoro #%1 completado").arg(pomodoro.completed());
            text = isLong ? QStringLiteral("Toma un descanso largo de %1 minutos.").arg(pomodoro.longMinutes())
                          : QStringLiteral("Toma un descanso corto de %1 minutos.").arg(pomodoro.shortMinutes());
        } else {
            title = QStringLiteral("Descanso terminado");
            text = QStringLiteral("Hora de volver al trabajo: %1 minutos de enfoque.").arg(pomodoro.focusMinutes());
        }
        KNotification::event(QStringLiteral("phaseFinished"), title, text, QStringLiteral("chronometer"),
                             pomodoro.autoStart() ? KNotification::CloseOnTimeout : KNotification::Persistent, QStringLiteral("kpomodoro"));
    });

    return app.exec();
}
