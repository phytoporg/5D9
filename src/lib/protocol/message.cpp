#include "message.h"

#include <cstring>

using namespace protocol;

MessageHeader::MessageHeader(MessageType type, size_t messageLen)
    : Type(type), MessageLen(messageLen)
{}

ConfigureMessage::ConfigureMessage(GameConfiguration* pConfigurations, uint32_t configurationCount)
    : Header(MessageType::Configure, sizeof(ConfigureMessage) + sizeof(GameConfiguration) * configurationCount),
      ConfigurationCount(configurationCount)
{
    memcpy(Configurations, pConfigurations, sizeof(GameConfiguration) * configurationCount);
}

LaunchMessage::LaunchMessage(const char *pGameName)
    : Header(MessageType::Launch, sizeof(LaunchMessage))
{}