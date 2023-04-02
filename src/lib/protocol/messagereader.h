#pragma once

#include "message.h"
#include <cstddef>
#include <common/memory/memorystream.h>

namespace protocol {
    const uint32_t kMessageReaderChunkSize = 512;
    using ReaderStream = common::memory::MemoryStream<kMessageReaderChunkSize>;
    class MessageReader
    {
    public:
        MessageReader(MessageHeader* pHeader, ReaderStream& readStream);

        // Returns 0 when done
        size_t ReadHeader();
        size_t ReadNext();
        size_t ChunksRemaining() const;
        size_t TotalBytesRead() const;

    private:
        const MessageHeader* m_pHeader = nullptr;
        uint8_t* m_pNext = nullptr;
        uint8_t* m_pLast = nullptr;
        size_t m_totalBytesRead = 0;
        ReaderStream& m_stream;
    };
}
