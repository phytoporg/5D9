#pragma once

#include <cstdint>
#include <cstddef>

namespace protocol {
    // General
    enum class MessageType
    {
        Invalid = 0,
        Configure,
        Launch,
        Max
    };

    static constexpr uint32_t kMessageMagic = 0xBEED00B;
    struct MessageHeader
    {
        MessageHeader(MessageType type, size_t messageLen);

        const uint32_t Magic = kMessageMagic;
        MessageType    Type  = MessageType::Invalid;
        size_t         MessageLen = 0;
    };

    // Configuration
    static constexpr uint32_t kGameNameMaxLength = 256;
    static constexpr uint32_t kGameCommandMaxLength = 1024;
    struct GameConfiguration
    {
        char Name[kGameNameMaxLength];
        char Command[kGameCommandMaxLength];
    };

    struct ConfigureMessage
    {
        ConfigureMessage(GameConfiguration* pConfigurations, uint32_t configurationCount);
        MessageHeader     Header;
        uint32_t          ConfigurationCount = 0;
        GameConfiguration Configurations[];
    };

    // Launch
    struct LaunchMessage
    {
        explicit LaunchMessage(const char* pGameName);
        MessageHeader Header;
        char Name[kGameNameMaxLength];
    };
}
