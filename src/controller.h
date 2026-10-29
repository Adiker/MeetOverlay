#pragma once
#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QLocalServer>
#include <QLocalSocket>
#include <QHash>
#include <QElapsedTimer>
#include <QTimer>
#include <QQuickWindow>

class Controller : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(QStringList screens READ screens NOTIFY screensChanged)
    Q_PROPERTY(int revision READ revision NOTIFY settingsChanged)
public:
    explicit Controller(bool server, QObject *parent = nullptr);
    QString state() const { return m_state; }
    QStringList screens() const;
    int revision() const { return m_revision; }
    Q_INVOKABLE QVariant get(const QString &key) const;
    Q_INVOKABLE void set(const QString &key, const QVariant &value);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void configureWindow(QObject *object);
    bool listening() const { return m_server.isListening(); }
signals:
    void stateChanged();
    void settingsChanged();
    void screensChanged();
private:
    struct Client { QString state = "unknown"; int count = 0; QElapsedTimer seen; };
    void reload();
    void updateState();
    void accept();
    void applyWindow();
    QSettings m_settings;
    QFileSystemWatcher m_watcher;
    QLocalServer m_server;
    QHash<QLocalSocket *, Client> m_clients;
    QTimer m_expiry;
    QString m_state = "unknown";
    int m_revision = 0;
    QQuickWindow *m_window = nullptr;
};
