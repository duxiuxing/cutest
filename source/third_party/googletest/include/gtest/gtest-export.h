#pragma once

#include "gtest/gtest.h"

namespace testing {

GTEST_API_ std::string FormatTestCount(int test_count);
static std::vector<std::string> GetReservedAttributesForElement(const std::string& xml_element);

} // namespace testing
