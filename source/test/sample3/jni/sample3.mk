LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := sample3

LOCAL_CPPFLAGS += \
    -std=c++11 \
    -frtti \
    -fexceptions

LOCAL_LDLIBS := -llog

# cppunit begin
CPPUNIT_INC_DIR := \
    $(LOCAL_PATH)/../../../third_party/cppunit/include
# cppunit end

# cutest begin
CUTEST_INC_DIR := \
    $(LOCAL_PATH)/../../../third_party/cutest/include
# cutest end

# gtest begin
GTEST_INC_DIR := \
    $(LOCAL_PATH)/../../../third_party/googletest/include
# gtest end

LOCAL_C_INCLUDES := \
    $(CPPUNIT_INC_DIR) \
    $(CUTEST_INC_DIR) \
    $(GTEST_INC_DIR)

LOCAL_SRC_FILES := \
	./../../../third_party/googletest/samples/sample3_unittest.cc

LOCAL_SHARED_LIBRARIES += libcutest

include $(BUILD_SHARED_LIBRARY)
