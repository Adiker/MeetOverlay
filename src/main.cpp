#include "controller.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QLockFile>
#include <QDebug>

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("MeetOverlay");
    if (QGuiApplication::platformName() != "wayland") {
        qCritical("MeetOverlay requires a native Wayland session."); return 1;
    }
    const bool settings = app.arguments().contains("--settings");
    QLockFile lock(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/meetoverlay.lock");
    if (!settings && !lock.tryLock(0)) { qWarning("MeetOverlay is already running"); return 1; }
    Controller controller(!settings);
    if (!settings && !controller.listening()) return 1;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &controller);
    const QUrl url(settings ? "qrc:/qml/Settings.qml" : "qrc:/qml/Overlay.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
