#include "native_framing.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QLocalSocket>
#include <QSocketNotifier>
#include <QStandardPaths>
#include <unistd.h>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QLocalSocket socket;
    const QString runtime = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    socket.connectToServer(runtime + "/meetoverlay.sock");
    if (!socket.waitForConnected(2000)) return 1;
    NativeFraming framing;
    QSocketNotifier input(STDIN_FILENO, QSocketNotifier::Read);
    QObject::connect(&socket, &QLocalSocket::disconnected, &app, &QCoreApplication::quit);
    QObject::connect(&input, &QSocketNotifier::activated, &app, [&] {
        char buffer[8192];
        const auto n = ::read(STDIN_FILENO, buffer, sizeof(buffer));
        if (n <= 0) { app.quit(); return; }
        QList<QByteArray> messages;
        if (!framing.feed(QByteArray(buffer, n), messages)) { app.exit(1); return; }
        for (const auto &message : messages) {
            QJsonParseError error;
            const auto json = QJsonDocument::fromJson(message, &error);
            if (error.error != QJsonParseError::NoError || !json.isObject()) { app.exit(1); return; }
            socket.write(json.toJson(QJsonDocument::Compact) + '\n');
        }
    });
    return app.exec();
}
