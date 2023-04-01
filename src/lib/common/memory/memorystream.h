#pragma once

#include <cstdint>
#include <cstddef>

#include <cstring>

namespace common { namespace memory {
    template<size_t TBufferSize>
    class MemoryStream
    {
    public:
        size_t Write(const void* pSource, size_t length);
        size_t Read(void* pDestination, size_t length);
        size_t Capacity() const;
        void* Get();

        bool Seek(size_t newPosition);
        size_t Tell() const;

    private:
        size_t  m_streamPosition = 0;
        uint8_t m_buffer[TBufferSize] = {};
    };

    template<size_t TBufferSize>
    size_t MemoryStream<TBufferSize>::Write(const void *pSource, size_t length)
    {
        const size_t BytesLeft = TBufferSize - m_streamPosition;
        const size_t BytesToWrite = BytesLeft < length ? BytesLeft : length;
        void* pDestination = static_cast<void*>(m_buffer + m_streamPosition);
        memcpy(pDestination, pSource, BytesToWrite);
        m_streamPosition += BytesToWrite;
        return BytesToWrite;
    }

    template<size_t TBufferSize>
    size_t MemoryStream<TBufferSize>::Read(void *pDestination, size_t length)
    {
        const size_t BytesLeft = TBufferSize - m_streamPosition;
        const size_t BytesToRead = BytesLeft < length ? BytesLeft : length;
        void* pSource = static_cast<void*>(m_buffer + m_streamPosition);
        memcpy(pDestination, pSource, BytesToRead);
        m_streamPosition += BytesToRead;
        return BytesToRead;
    }

    template<size_t TBufferSize>
    size_t MemoryStream<TBufferSize>::Capacity() const {
        return TBufferSize;
    }

    template<size_t TBufferSize>
    void* MemoryStream<TBufferSize>::Get() {
        return m_buffer;
    }

    template<size_t TBufferSize>
    size_t MemoryStream<TBufferSize>::Tell() const {
        return m_streamPosition;
    }

    template<size_t TBufferSize>
    bool MemoryStream<TBufferSize>::Seek(size_t newPosition) {
        if (newPosition >= TBufferSize)
        {
            return false;
        }

        return m_streamPosition = newPosition;
    }
}}
