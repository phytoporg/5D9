#include "messagereader.h"

using namespace protocol;

MessageReader::MessageReader(ReaderStream &readStream)
    : m_stream(readStream)
{}

size_t MessageReader::ReadMessage(MessageHeader *pHeader)
{
    return m_stream.Read(pHeader, pHeader->MessageLen);
}
