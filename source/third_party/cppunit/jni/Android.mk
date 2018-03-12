LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := cppunit

LOCAL_CPPFLAGS += \
    -fexceptions

LOCAL_LDLIBS := -llog

#cppunit begin
CPPUNIT_INC_DIR := \
    $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/../src/cppunit
#cppunit end

#gtest begin
GTEST_INC_DIR := \
    $(LOCAL_PATH)/../../../googletest \
    $(LOCAL_PATH)/../../../googletest/include
#gtest end

LOCAL_C_INCLUDES := \
    $(CPPUNIT_INC_DIR) \
    $(GTEST_INC_DIR)

LOCAL_SRC_FILES := \
    ./../src/cppunit/android/cppunit_all.cpp \
    ./../../../googletest/src/gtest-all.cc

include $(BUILD_SHARED_LIBRARY)