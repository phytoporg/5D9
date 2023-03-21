#pragma once

#include "log.h"

#define RELEASE_CHECK(Condition, String, ...) \
    if (!(Condition)) { \
        RELEASE_LOGLINE_FATAL(LOG_DEFAULT, String __VA_OPT__(,) __VA_ARGS__); \
    }
