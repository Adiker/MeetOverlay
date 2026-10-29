import QtQuick
import QtQuick.Window
Window {
    id: win
    property bool ready: false
    readonly property int rev: controller.revision
    readonly property string mode: { rev; return controller.get("mode") }
    readonly property int base: { rev; return Math.max(32, Math.min(200, controller.get("size"))) }
    width: mode === "icon" ? base : base * 2.8
    height: base
    visible: { rev; let hidden = controller.get("hideUnknown"); return ready && (!(hidden === true || hidden === "true") || controller.state !== "unknown") }
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus | Qt.WindowTransparentForInput
    Component.onCompleted: { controller.configureWindow(win); ready = true }
    SystemPalette { id: systemPalette; colorGroup: SystemPalette.Active }
    Rectangle {
        anchors.fill: parent
        color: { win.rev; let theme = controller.get("autoTheme"); let c = Qt.color(theme === true || theme === "true" ? systemPalette.window : controller.get("background")); c.a = controller.get("opacity"); return c }
        radius: { win.rev; return win.mode === "capsule" ? height/2 : controller.get("radius") }
        border.color: { win.rev; return controller.get("border") }
        border.width: 1
        Behavior on color { enabled: controller.get("animation") === true || controller.get("animation") === "true"; ColorAnimation { duration: 150 } }
        Indicator { width: win.base; height: win.base; anchors.left: parent.left; state: controller.state }
        Text {
            visible: win.mode !== "icon"; anchors.left: parent.left; anchors.leftMargin: win.base; anchors.verticalCenter: parent.verticalCenter
            text: controller.state === "muted" ? qsTr("Muted") : controller.state === "active" ? qsTr("Live") : qsTr("Unknown")
            color: { win.rev; return controller.state === "muted" ? controller.get("muted") : controller.state === "active" ? controller.get("active") : controller.get("unknown") }
            font.pixelSize: Math.max(12, win.base * 0.22); font.bold: true
        }
    }
}
