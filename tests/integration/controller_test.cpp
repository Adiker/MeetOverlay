#include "controller.h"
#include <QtTest>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QLocalSocket>

class ControllerTest : public QObject {
    Q_OBJECT
private slots:
    void socketAndSettings() {
        QTemporaryDir dir; QVERIFY(dir.isValid());
        qputenv("XDG_CONFIG_HOME", dir.path().toUtf8());
        qputenv("XDG_RUNTIME_DIR", dir.path().toUtf8());
        Controller c(true); QVERIFY(c.listening());
        QCOMPARE(c.state(), QString("unknown"));
        c.set("margin", 37); QCOMPARE(c.get("margin").toInt(), 37);
        QLocalSocket a, b;
        const auto path = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation) + "/meetoverlay.sock";
        a.connectToServer(path); QVERIFY(a.waitForConnected(1000));
        a.write("{\"state\":\"active\",\"count\":1}\n"); a.flush();
        QTRY_COMPARE(c.state(), QString("active"));
        b.connectToServer(path); QVERIFY(b.waitForConnected(1000));
        b.write("{\"state\":\"muted\",\"count\":1}\n"); b.flush();
        QTRY_COMPARE(c.state(), QString("unknown"));
        b.disconnectFromServer();
        QTRY_COMPARE(c.state(), QString("active"));
        a.write("{\"state\":\"muted\",\"count\":1}\n"); a.flush();
        QTRY_COMPARE(c.state(), QString("muted"));
        a.disconnectFromServer();
        QTRY_COMPARE(c.state(), QString("unknown"));
        c.reset(); QCOMPARE(c.get("margin").toInt(), 24);
    }
};
QTEST_MAIN(ControllerTest)
#include "controller_test.moc"
