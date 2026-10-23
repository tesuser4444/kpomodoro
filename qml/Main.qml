import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kpomodoro.app

Kirigami.ApplicationWindow {
    id: root

    title: "Pomodoro"
    width: 380
    height: 540
    minimumWidth: 320
    minimumHeight: 460

    readonly property color phaseColor: Pomodoro.phase === Pomodoro.Focus
        ? Kirigami.Theme.negativeTextColor
        : Kirigami.Theme.positiveTextColor

    function clock(s) {
        return String(Math.floor(s / 60)).padStart(2, "0") + ":" + String(s % 60).padStart(2, "0")
    }

    pageStack.initialPage: Kirigami.Page {
        title: Pomodoro.phase === Pomodoro.Focus ? "Enfoque"
             : Pomodoro.phase === Pomodoro.ShortBreak ? "Descanso corto" : "Descanso largo"

        actions: [
            Kirigami.Action {
                icon.name: "configure"
                text: "Ajustes"
                onTriggered: settings.open()
            },
            Kirigami.Action {
                icon.name: "application-exit"
                text: "Salir"
                onTriggered: Qt.quit()
            }
        ]

        ColumnLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Canvas {
                    id: ring
                    readonly property real fraction: Pomodoro.total > 0 ? Pomodoro.remaining / Pomodoro.total : 0
                    readonly property color track: Kirigami.Theme.alternateBackgroundColor
                    anchors.centerIn: parent
                    width: Math.min(parent.width, parent.height)
                    height: width

                    onFractionChanged: requestPaint()
                    onTrackChanged: requestPaint()
                    Connections {
                        target: root
                        function onPhaseColorChanged() { ring.requestPaint() }
                    }

                    onPaint: {
                        const ctx = getContext("2d")
                        const lw = Kirigami.Units.gridUnit * 0.8
                        const r = width / 2 - lw
                        const c = width / 2
                        ctx.reset()
                        ctx.lineWidth = lw
                        ctx.lineCap = "round"
                        ctx.strokeStyle = track
                        ctx.beginPath()
                        ctx.arc(c, c, r, 0, 2 * Math.PI)
                        ctx.stroke()
                        if (fraction > 0) {
                            ctx.strokeStyle = root.phaseColor
                            ctx.beginPath()
                            ctx.arc(c, c, r, -Math.PI / 2, -Math.PI / 2 + 2 * Math.PI * fraction)
                            ctx.stroke()
                        }
                    }

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 0
                        Kirigami.Heading {
                            Layout.alignment: Qt.AlignHCenter
                            text: root.clock(Pomodoro.remaining)
                            font.pixelSize: ring.width * 0.2
                            font.weight: Font.Light
                            font.features: { "tnum": 1 }
                            level: 1
                        }
                        Controls.Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: Pomodoro.completed + " completados"
                            opacity: 0.7
                        }
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: Kirigami.Units.largeSpacing

                Controls.Button {
                    icon.name: "view-refresh"
                    display: Controls.AbstractButton.IconOnly
                    Controls.ToolTip.text: "Reiniciar fase"
                    Controls.ToolTip.visible: hovered
                    onClicked: Pomodoro.reset()
                }
                Controls.Button {
                    highlighted: true
                    icon.name: Pomodoro.running ? "media-playback-pause" : "media-playback-start"
                    text: Pomodoro.running ? "Pausar" : "Iniciar"
                    onClicked: Pomodoro.toggle()
                }
                Controls.Button {
                    icon.name: "media-skip-forward"
                    display: Controls.AbstractButton.IconOnly
                    Controls.ToolTip.text: "Saltar fase"
                    Controls.ToolTip.visible: hovered
                    onClicked: Pomodoro.skip()
                }
            }
        }
    }

    Kirigami.OverlaySheet {
        id: settings
        title: "Ajustes"

        Kirigami.FormLayout {
            Controls.SpinBox {
                Kirigami.FormData.label: "Enfoque (min):"
                from: 1; to: 180
                value: Pomodoro.focusMinutes
                onValueModified: { Pomodoro.focusMinutes = value; value = Qt.binding(() => Pomodoro.focusMinutes) }
            }
            Controls.SpinBox {
                Kirigami.FormData.label: "Descanso corto (min):"
                from: 1; to: 60
                value: Pomodoro.shortMinutes
                onValueModified: { Pomodoro.shortMinutes = value; value = Qt.binding(() => Pomodoro.shortMinutes) }
            }
            Controls.SpinBox {
                Kirigami.FormData.label: "Descanso largo (min):"
                from: 1; to: 120
                value: Pomodoro.longMinutes
                onValueModified: { Pomodoro.longMinutes = value; value = Qt.binding(() => Pomodoro.longMinutes) }
            }
            Controls.SpinBox {
                Kirigami.FormData.label: "Descanso largo cada:"
                from: 2; to: 12
                value: Pomodoro.longEvery
                onValueModified: { Pomodoro.longEvery = value; value = Qt.binding(() => Pomodoro.longEvery) }
            }
            Controls.CheckBox {
                text: "Iniciar la siguiente fase automáticamente"
                checked: Pomodoro.autoStart
                onToggled: Pomodoro.autoStart = checked
            }
        }
    }
}
