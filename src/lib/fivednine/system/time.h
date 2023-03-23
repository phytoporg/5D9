#pragma once

#include <cstdint>

namespace fivednine { namespace system { namespace time {
    uint32_t GetTicksMs();
    void SleepMs(uint32_t milliseconds);
}}}