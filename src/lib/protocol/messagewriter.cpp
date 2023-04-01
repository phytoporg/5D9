#include "messagewriter.h"

using namespace protocol;

MessageWriter::MessageWriter(WriterStream& writeStream)
    : m_stream(writeStream)
{}

size_t MessageWriter::WriteMessage(const MessageHeader* pHeader)
{
    return m_stream.Write(pHeader, pHeader->MessageLen);
}
