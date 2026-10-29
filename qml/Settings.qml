import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
ApplicationWindow {
    id: window
    width: 720; height: 760; minimumWidth: 550; minimumHeight: 500; visible: true
    title: qsTr("MeetOverlay Settings")
    property int revision: controller.revision
    property string previewState: ["muted", "active", "unknown"][previewChoice.currentIndex]
    function value(key) { revision; return controller.get(key) }
    SystemPalette { id: systemPalette; colorGroup: SystemPalette.Active }
    ColorDialog { id: colorDialog; property string key; onAccepted: controller.set(key, selectedColor.toString()) }
    header: ToolBar { RowLayout { anchors.fill: parent; Label { text: qsTr("MeetOverlay Settings"); font.bold: true; Layout.fillWidth: true; leftPadding: 12 } Button { text: qsTr("Restore defaults"); onClicked: controller.reset() } } }
    ScrollView {
      anchors.fill: parent; contentWidth: availableWidth
      ColumnLayout {
        width: parent.width; spacing: 12
        RowLayout {
          Layout.fillWidth: true; Layout.margins: 12
          Label { text: qsTr("Appearance preview"); font.bold: true; Layout.fillWidth: true }
          ComboBox { id: previewChoice; model: [qsTr("Muted"), qsTr("Live"), qsTr("Unknown")]; currentIndex: 0; Accessible.name: qsTr("Preview state") }
        }
        Label { text: qsTr("Preview only — does not show the current microphone state"); Layout.leftMargin: 12; Layout.rightMargin: 12; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Rectangle {
          Layout.alignment: Qt.AlignHCenter
          readonly property int base: Number(window.value("size"))
          readonly property string mode: window.value("mode")
          width: mode === "icon" ? base : base * 2.8; height: base
          radius: mode === "capsule" ? height / 2 : window.value("radius")
          color: { let automatic = window.value("autoTheme"); let c = Qt.color(automatic === true || automatic === "true" ? systemPalette.window : window.value("background")); c.a = window.value("opacity"); return c }
          border.color: window.value("border")
          Indicator { width: parent.base; height: parent.base; state: window.previewState }
          Text { visible: parent.mode !== "icon"; anchors.left: parent.left; anchors.leftMargin: parent.base; anchors.verticalCenter: parent.verticalCenter; text: window.previewState === "muted" ? qsTr("Muted") : window.previewState === "active" ? qsTr("Live") : qsTr("Unknown"); color: window.value(window.previewState); font.pixelSize: Math.max(12, parent.base * .22); font.bold: true }
        }
        GridLayout {
          columns: 2; columnSpacing: 15; rowSpacing: 8; Layout.fillWidth: true; Layout.margins: 16
          Label { text: qsTr("Monitor") }
          ComboBox { Layout.fillWidth: true; model: [qsTr("Primary")].concat(controller.screens); currentIndex: Math.max(0, controller.screens.indexOf(window.value("screen")) + 1); onActivated: controller.set("screen", index === 0 ? "" : controller.screens[index-1]) }
          Label { text: qsTr("Position") }
          ComboBox { Layout.fillWidth: true; model: ["top-left", "top-center", "top-right", "bottom-left", "bottom-center", "bottom-right"]; currentIndex: Math.max(0, model.indexOf(window.value("position"))); onActivated: controller.set("position", currentText) }
          Label { text: qsTr("Margin") }
          SpinBox { from: 0; to: 500; value: window.value("margin"); onValueModified: controller.set("margin", value) }
          Label { text: qsTr("Horizontal offset") }
          SpinBox { from: -500; to: 500; value: window.value("offsetX"); onValueModified: controller.set("offsetX", value) }
          Label { text: qsTr("Vertical offset") }
          SpinBox { from: -500; to: 500; value: window.value("offsetY"); onValueModified: controller.set("offsetY", value) }
          Label { text: qsTr("Size") }
          SpinBox { from: 32; to: 200; value: window.value("size"); onValueModified: controller.set("size", value) }
          Label { text: qsTr("Icon size") }
          SpinBox { from: 12; to: 160; value: window.value("iconSize"); onValueModified: controller.set("iconSize", value) }
          Label { text: qsTr("Background opacity") }
          Slider { from: 0; to: 1; stepSize: .05; value: window.value("opacity"); onMoved: controller.set("opacity", value); Layout.fillWidth: true }
          Label { text: qsTr("Corner radius") }
          SpinBox { from: 0; to: 100; value: window.value("radius"); onValueModified: controller.set("radius", value) }
          Label { text: qsTr("Appearance") }
          ComboBox { model: ["icon", "text", "capsule"]; currentIndex: Math.max(0, model.indexOf(window.value("mode"))); onActivated: controller.set("mode", currentText) }
          Repeater {
            model: [{label: qsTr("Background"), key: "background"}, {label: qsTr("Border"), key: "border"}, {label: qsTr("Muted"), key: "muted"}, {label: qsTr("Active"), key: "active"}, {label: qsTr("Unknown"), key: "unknown"}]
            delegate: RowLayout {
              required property var modelData
              Layout.columnSpan: 2; Layout.fillWidth: true
              Label { text: modelData.label; Layout.fillWidth: true }
              Button { text: window.value(modelData.key); onClicked: { colorDialog.key = modelData.key; colorDialog.selectedColor = window.value(modelData.key); colorDialog.open() } }
            }
          }
          Label { text: qsTr("Follow system theme for background") }
          Switch { checked: window.value("autoTheme") === true || window.value("autoTheme") === "true"; onToggled: controller.set("autoTheme", checked) }
          Label { text: qsTr("Hide when state is unknown") }
          Switch { checked: window.value("hideUnknown") === true || window.value("hideUnknown") === "true"; onToggled: controller.set("hideUnknown", checked) }
          Label { text: qsTr("Animations") }
          Switch { checked: window.value("animation") === true || window.value("animation") === "true"; onToggled: controller.set("animation", checked) }
        }
      }
    }
}
