import QtQuick
Item {
    id: root
    property string state: "unknown"
    property color stateColor: { controller.revision; return state === "muted" ? controller.get("muted") : state === "active" ? controller.get("active") : controller.get("unknown") }
    property bool animate: true
    Behavior on stateColor { enabled: root.animate && (controller.get("animation") === true || controller.get("animation") === "true"); ColorAnimation { duration: 150 } }
    Canvas {
        id: icon
        anchors.centerIn: parent
        width: Math.min(parent.width, (controller.revision, controller.get("iconSize")))
        height: width
        onPaint: {
            let ctx = getContext("2d"); ctx.reset();
            ctx.strokeStyle = root.stateColor; ctx.lineWidth = width * 0.085; ctx.lineCap = "round";
            ctx.beginPath(); ctx.roundedRect(width*0.36,height*0.08,width*0.28,height*0.52,width*0.14,width*0.14); ctx.stroke();
            ctx.beginPath(); ctx.arc(width*0.5,height*0.43,width*0.3,0,Math.PI,false); ctx.moveTo(width*0.5,height*0.73); ctx.lineTo(width*0.5,height*0.91); ctx.moveTo(width*0.33,height*0.91); ctx.lineTo(width*0.67,height*0.91); ctx.stroke();
            if (root.state !== "active") { ctx.beginPath(); ctx.moveTo(width*0.14,height*0.12); ctx.lineTo(width*0.86,height*0.88); ctx.stroke(); }
        }
        Connections { target: root; function onStateColorChanged() { icon.requestPaint() } function onStateChanged() { icon.requestPaint() } }
        Connections { target: controller; function onSettingsChanged() { icon.requestPaint() } }
    }
}
