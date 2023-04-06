#pragma once

#include "message.h"
#include <cstddef>
#include <common/memory/memorystream.h>

namespace protocol {
    const uint32_t kMessageWriterChunkSize = 512;
    using WriterStream = common::memory::MemoryStream<kMessageWriterChunkSize>;
    class MessageWriter
    {
    public:
        MessageWriter(const MessageHeader* pHeader, WriterStream& writeStream);

        // Returns 0 when done
        size_t WriteNext();
        size_t ChunksRemaining() const;
        size_t TotalBytesWritten() const;

    private:
        const MessageHeader* m_pHeader = nullptr;
        const uint8_t* m_pNext = nullptr;
        const uint8_t* m_pLast = nullptr;
        size_t m_totalBytesWritten = 0;
        WriterStream& m_stream;
    };
}