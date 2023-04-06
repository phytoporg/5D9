#include "messagewriter.h"
#include <algorithm>

using namespace protocol;

MessageWriter::MessageWriter(const MessageHeader* pHeader, WriterStream& writeStream)
    : m_pHeader(pHeader),
      m_pNext(reinterpret_cast<const uint8_t*>(pHeader)),
      m_pLast(m_pNext + pHeader->MessageLen),
      m_stream(writeStream)
{}

size_t MessageWriter::WriteNext()
{
    const size_t BytesRemaining = m_pLast - m_pNext;
    const size_t BytesToWrite = std::min({
        BytesRemaining,
        m_stream.Capacity() - m_stream.Tell()
    });

    if (!BytesToWrite)
    {
        return 0;
    }

    const ssize_t BytesWritten = m_stream.Write(m_pNext, BytesToWrite);
    if (BytesWritten > 0)
    {
        m_pNext += BytesWritten;
        m_totalBytesWritten += BytesWritten;
    }

    return BytesWritten;
}

size_t MessageWriter::ChunksRemaining() const
{
    if (m_pNext >= m_pLast)
    {
        return 0;
    }

    const size_t BytesRemaining = m_pLast - m_pNext;
    const size_t Quotient = BytesRemaining / m_stream.Capacity();
    const size_t Remainder = BytesRemaining % m_stream.Capacity();

    return Quotient + (Remainder > 0 ? 1 : 0);
}

size_t MessageWriter::TotalBytesWritten() const
{
    return m_totalBytesWritten;
}
