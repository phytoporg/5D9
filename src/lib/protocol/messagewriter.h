#pragma once

#include "message.h"
#include <cstddef>
#include <common/memory/memorystream.h>

namespace protocol {
    using WriterStream = common::memory::MemoryStream<512>;
    class MessageWriter
    {
    public:
        MessageWriter(WriterStream& writeStream);
        size_t WriteMessage(const MessageHeader* pHeader);

        template<typename TMessage>
        size_t WriteMessage(const TMessage& message)
        {
            return WriteMessage(&(message.Header));
        }

    private:
        WriterStream& m_stream;
    };
}