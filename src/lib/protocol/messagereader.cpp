#include "messagereader.h"
#include <algorithm>

using namespace protocol;

MessageReader::MessageReader(MessageHeader* pHeader, ReaderStream &readStream)
    : m_pHeader(pHeader),
      m_pNext(reinterpret_cast<uint8_t*>(pHeader)),
      m_pLast(m_pNext + pHeader->MessageLen),
      m_stream(readStream)
{}

size_t MessageReader::ReadNext()
{
    const size_t BytesRemaining = m_pLast - m_pNext;
    const size_t BytesToRead = std::min({
        BytesRemaining,
        m_stream.Capacity() - m_stream.Tell()
    });

    if (!BytesToRead)
    {
        return 0;
    }

    const ssize_t BytesRead = m_stream.Read(m_pNext, BytesToRead);
    if (BytesRead > 0)
    {
        m_pNext += BytesRead;
        m_totalBytesRead += BytesRead;
    }

    return BytesRead;
}

size_t MessageReader::ChunksRemaining() const
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

size_t MessageReader::TotalBytesRead() const
{
    return m_totalBytesRead;
}

