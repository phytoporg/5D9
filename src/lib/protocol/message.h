#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>

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
        MessageHeader() = default;
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
        GameConfiguration();
        GameConfiguration(const char* pTitle, const char* pCommand);
        char Name[kGameNameMaxLength];
        char Command[kGameCommandMaxLength];
    };

    constexpr uint8_t kMaxNumConfigurations = 255;
    struct ConfigureMessage
    {
        ConfigureMessage(GameConfiguration* pConfigurations, uint8_t configurationCount);
        MessageHeader     Header;
        uint8_t           ConfigurationCount = 0;
        GameConfiguration Configurations[kMaxNumConfigurations];
    };

    // Launch
    struct LaunchMessage
    {
        explicit LaunchMessage(const char* pGameName);
        MessageHeader Header;
        char Name[kGameNameMaxLength];
    };
}
