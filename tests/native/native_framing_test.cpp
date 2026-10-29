#include "native_framing.h"
#include <QtTest>
#include <cstring>

class FramingTest : public QObject {
    Q_OBJECT
private slots:
    void fragmentedAndCoalesced() {
        const auto first = NativeFraming::encode("{\"x\":1}");
        const auto second = NativeFraming::encode("{}");
        NativeFraming decoder;
        QList<QByteArray> messages;
        QVERIFY(decoder.feed(first.left(2), messages));
        QVERIFY(messages.isEmpty());
        QVERIFY(decoder.feed(first.mid(2) + second, messages));
        QCOMPARE(messages, QList<QByteArray>({"{\"x\":1}", "{}"}));
    }
    void rejectsInvalidLengths() {
        for (quint32 length : {quint32(0), NativeFraming::MaxMessage + 1}) {
            QByteArray header(reinterpret_cast<const char *>(&length), sizeof(length));
            NativeFraming decoder;
            QList<QByteArray> messages;
            QVERIFY(!decoder.feed(header, messages));
            QVERIFY(decoder.failed());
            QVERIFY(!decoder.feed(NativeFraming::encode("{}"), messages));
            QVERIFY(messages.isEmpty());
        }
    }
};
QTEST_GUILESS_MAIN(FramingTest)
#include "native_framing_test.moc"
