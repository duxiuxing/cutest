#pragma once

#include "gtest/gtest.h"

namespace testing {

GTEST_API_ std::string FormatTestCount(int test_count);
static std::vector<std::string> GetReservedAttributesForElement(const std::string& xml_element);

namespace internal {

enum GTestColor {
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW
};

GTEST_API_ void ColoredPrintf(GTestColor color, const char* fmt, ...);

} // namespace internal

} // namespace testing