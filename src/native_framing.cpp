#include "native_framing.h"
#include <cstring>

bool NativeFraming::feed(const QByteArray &bytes, QList<QByteArray> &messages)
{
    if (m_failed) return false;
    m_buffer.append(bytes);
    for (;;) {
        if (!m_haveHeader) {
            if (m_buffer.size() < 4) return true;
            std::memcpy(&m_expected, m_buffer.constData(), sizeof(m_expected));
            m_buffer.remove(0, 4);
            if (m_expected == 0 || m_expected > MaxMessage) {
                m_failed = true;
                m_buffer.clear();
                return false;
            }
            m_haveHeader = true;
        }
        if (m_buffer.size() < m_expected) return true;
        messages.append(m_buffer.left(m_expected));
        m_buffer.remove(0, m_expected);
        m_haveHeader = false;
    }
}

QByteArray NativeFraming::encode(const QByteArray &json)
{
    if (json.isEmpty() || json.size() > MaxMessage) return {};
    const quint32 length = static_cast<quint32>(json.size());
    QByteArray result(reinterpret_cast<const char *>(&length), sizeof(length));
    result.append(json);
    return result;
}
