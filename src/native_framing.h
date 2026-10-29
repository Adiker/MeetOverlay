#pragma once
#include <QByteArray>
#include <QList>
#include <QtGlobal>

// Incremental decoder for the browser-to-host native messaging byte stream.
// An invalid length is fatal: callers must discard the connection.
class NativeFraming {
public:
    static constexpr quint32 MaxMessage = 1024 * 1024;
    bool feed(const QByteArray &bytes, QList<QByteArray> &messages);
    bool failed() const { return m_failed; }
    static QByteArray encode(const QByteArray &json);
private:
    QByteArray m_buffer;
    quint32 m_expected = 0;
    bool m_haveHeader = false;
    bool m_failed = false;
};
