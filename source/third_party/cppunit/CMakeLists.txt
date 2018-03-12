cmake_minimum_required(VERSION 3.4.1)

# 在以下位置检索.h文件
include_directories(
    include
    src
    ../../googletest
    ../../googletest/include)

# 编译libcppunit.so
add_library(
    cppunit
    SHARED
    src/cppunit/android/cppunit_all.cpp
    ../../googletest/src/gtest-all.cc)

# 支持在代码里面调用__android_log_print
target_link_libraries(
    cppunit
    log)
