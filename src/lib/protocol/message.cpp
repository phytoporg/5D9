#include "message.h"

#include <cstring>

#include <common/log/check.h>

using namespace protocol;

MessageHeader::MessageHeader(MessageType type, size_t messageLen)
    : Type(type), MessageLen(messageLen)
{}

GameConfiguration::GameConfiguration()
{
    memset(Name, 0, sizeof(Name));
    memset(Command, 0, sizeof(Command));
}

GameConfiguration::GameConfiguration(const char *pTitle, const char *pCommand)
    : GameConfiguration()
{
    strncpy(Name, pTitle, sizeof(Name));
    strncpy(Command, pCommand, sizeof(Command));
}

ConfigureMessage::ConfigureMessage(GameConfiguration* pConfigurations, uint8_t configurationCount)
    : Header(MessageType::Configure, sizeof(Header) + sizeof(ConfigurationCount) + sizeof(GameConfiguration) * configurationCount),
      ConfigurationCount(configurationCount)
{
    RELEASE_CHECK(configurationCount < kMaxNumConfigurations, "Exceeded maximum configuration count");
    memcpy(Configurations, pConfigurations, sizeof(GameConfiguration) * configurationCount);
}

LaunchMessage::LaunchMessage(const char *pGameName)
    : Header(MessageType::Launch, sizeof(LaunchMessage))
{
    strncpy(Name, pGameName, sizeof(Name));
}
