#pragma once

#include "message.h"
#include <cstddef>
#include <common/memory/memorystream.h>

namespace protocol {
    using ReaderStream = common::memory::MemoryStream<512>;
    class MessageReader
    {
    public:
        explicit MessageReader(ReaderStream& readStream);
        size_t ReadMessage(MessageHeader* pHeader);

        template<typename TMessage>
        size_t ReadMessage(const TMessage& message)
        {
            return ReadMessage(&message.Header);
        }

    private:
        ReaderStream& m_stream;
    };
}
