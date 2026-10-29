#include "controller.h"
#include <LayerShellQt/Window>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScreen>
#include <QStandardPaths>
#include <QDir>
#include <QTimer>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(meetoverlayStateLog, "meetoverlay.state", QtWarningMsg)

static QVariant defaultValue(const QString &key) {
    static const QHash<QString, QVariant> defaults = {
        {"screen", ""}, {"position", "top-right"}, {"margin", 24}, {"offsetX", 0}, {"offsetY", 0},
        {"size", 64}, {"iconSize", 30}, {"opacity", 0.85}, {"background", "#25282e"},
        {"border", "#ffffff"}, {"radius", 16}, {"mode", "icon"}, {"muted", "#ef4444"},
        {"active", "#22c55e"}, {"unknown", "#9ca3af"}, {"animation", true}, {"hideUnknown", false}, {"autoTheme", true}
    };
    return defaults.value(key);
}
Controller::Controller(bool server, QObject *parent)
    : QObject(parent), m_settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/meetoverlay/settings.ini", QSettings::IniFormat)
{
    QDir().mkpath(QFileInfo(m_settings.fileName()).absolutePath());
    m_watcher.addPath(QFileInfo(m_settings.fileName()).absolutePath());
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this] { reload(); });
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, [this] { reload(); });
    connect(qApp, &QGuiApplication::screenAdded, this, [this] { emit screensChanged(); applyWindow(); });
    connect(qApp, &QGuiApplication::screenRemoved, this, [this] { emit screensChanged(); QTimer::singleShot(0, this, [this]{ applyWindow(); }); });
    if (!server) return;
    m_server.setSocketOptions(QLocalServer::UserAccessOption);
    const auto path = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/meetoverlay.sock";
    // The lock is taken by main before stale socket removal.
    QLocalServer::removeServer(path);
    if (!m_server.listen(path)) qWarning("MeetOverlay: cannot listen on local socket");
    connect(&m_server, &QLocalServer::newConnection, this, &Controller::accept);
    connect(&m_expiry, &QTimer::timeout, this, &Controller::updateState);
    m_expiry.start(5000);
}
QStringList Controller::screens() const {
    QStringList result; for (const auto s : QGuiApplication::screens()) result << s->name(); return result;
}
QVariant Controller::get(const QString &key) const {
    const auto fallback = defaultValue(key);
    return fallback.isValid() ? m_settings.value(key, fallback) : QVariant();
}
void Controller::set(const QString &key, const QVariant &value) {
    if (!defaultValue(key).isValid()) return;
    m_settings.setValue(key, value); m_settings.sync(); reload();
}
void Controller::reset() { m_settings.clear(); m_settings.sync(); reload(); }
void Controller::reload() {
    m_settings.sync();
    if (QFile::exists(m_settings.fileName()) && !m_watcher.files().contains(m_settings.fileName())) m_watcher.addPath(m_settings.fileName());
    ++m_revision; emit settingsChanged(); applyWindow();
}
void Controller::configureWindow(QObject *object) {
    m_window = qobject_cast<QQuickWindow *>(object); applyWindow();
}
void Controller::applyWindow() {
    if (!m_window) return;
    auto layer = LayerShellQt::Window::get(m_window);
    layer->setLayer(LayerShellQt::Window::LayerOverlay);
    layer->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
    layer->setExclusiveZone(-1); layer->setActivateOnShow(false);
    QScreen *screen = nullptr;
    for (auto s : QGuiApplication::screens()) if (s->name() == get("screen").toString()) screen = s;
    if (!screen) screen = QGuiApplication::primaryScreen();
    if (screen) layer->setScreen(screen);
    using W = LayerShellQt::Window;
    const QString pos = get("position").toString();
    W::Anchors anchors = pos.startsWith("bottom") ? W::AnchorBottom : W::AnchorTop;
    if (pos.endsWith("left")) anchors |= W::AnchorLeft;
    if (pos.endsWith("right")) anchors |= W::AnchorRight;
    if (pos.endsWith("center")) { anchors |= W::AnchorLeft; anchors |= W::AnchorRight; }
    layer->setAnchors(anchors);
    const int margin = qBound(0, get("margin").toInt(), 500);
    const int x = qBound(-500, get("offsetX").toInt(), 500), y = qBound(-500, get("offsetY").toInt(), 500);
    layer->setMargins(QMargins(qMax(0, margin+x), qMax(0, margin+y), qMax(0, margin-x), qMax(0, margin-y)));
    m_window->setFlag(Qt::WindowTransparentForInput, true);
}
void Controller::accept() {
    while (auto socket = m_server.nextPendingConnection()) {
        Client client; client.seen.start(); m_clients.insert(socket, client);
        connect(socket, &QLocalSocket::readyRead, this, [this, socket] {
            if (socket->bytesAvailable() > 65536) { socket->disconnectFromServer(); return; }
            while (socket->canReadLine()) {
                const auto object = QJsonDocument::fromJson(socket->readLine()).object();
                const auto state = object.value("state").toString(); const auto count = object.value("count");
                if ((state != "active" && state != "muted" && state != "unknown") || !count.isDouble() || count.toInt(-1) < 0 || count.toInt(-1) > 1000) {
                    socket->disconnectFromServer(); return;
                }
                qCDebug(meetoverlayStateLog) << "Received state" << state << "tab count" << count.toInt();
                auto &client = m_clients[socket]; client.state = state; client.count = count.toInt(); client.seen.restart();
            }
            updateState();
        });
        connect(socket, &QLocalSocket::disconnected, this, [this, socket] { m_clients.remove(socket); socket->deleteLater(); updateState(); });
    }
}
void Controller::updateState() {
    int count = 0; QString state = "unknown";
    for (const auto &client : std::as_const(m_clients)) {
        if (!client.seen.isValid() || client.seen.elapsed() > 50000) { count += 2; continue; }
        count += client.count; if (client.count == 1) state = client.state;
    }
    if (count != 1) state = "unknown";
    if (m_state != state) { m_state = state; qCDebug(meetoverlayStateLog) << "Overlay state" << state; emit stateChanged(); }
}
