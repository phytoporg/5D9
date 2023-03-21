#pragma once

#include <countdown/log/log.h>

#define RELEASE_CHECK(Condition, FailureText) if (!(Condition)) { countdown::log::LogAndFail(FailureText); }
