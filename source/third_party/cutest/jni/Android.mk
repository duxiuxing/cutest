LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := cutest

LOCAL_CFLAGS := \
    -D_CUTEST_IMPL

LOCAL_CPPFLAGS += \
    -std=c++11 \
    -frtti \
    -fexceptions

LOCAL_LDLIBS := -llog

#cppunit begin
CPPUNIT_INC_DIR := \
    $(LOCAL_PATH)/../../cppunit/include \
    $(LOCAL_PATH)/../../cppunit/src/cppunit
#cppunit end

#cutest begin
CUTEST_INC_DIR := \
    $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/..
#cutest end

#gmock begin
GMOCK_INC_DIR := \
    $(LOCAL_PATH)/../../googlemock/include \
    $(LOCAL_PATH)/../../googlemock
#gmock end

#gtest begin
GTEST_INC_DIR := \
    $(LOCAL_PATH)/../../googletest/include \
    $(LOCAL_PATH)/../../googletest
#gtest end

LOCAL_C_INCLUDES := \
    $(CPPUNIT_INC_DIR) \
    $(CUTEST_INC_DIR) \
    $(GMOCK_INC_DIR) \
    $(GTEST_INC_DIR)

LOCAL_SRC_FILES := \
    ./../../cppunit/src/cppunit/cppunit-all.cpp \
    ./../../googletest/src/gtest-all.cc \
	./../../googlemock/src/gmock-all.cc \
	./../src/runner_base.cpp \
	./../src/android/decorator_impl.cpp \
    ./../src/android/event_impl.cpp \
	./../src/android/JClassManager.cpp \
	./../src/android/JniEnv.cpp \
	./../src/android/JniListener.cpp \
	./../src/android/logger.cpp \	
	./../src/android/runner_impl.cpp \
	./../src/android/synchronization_object_impl.cpp
	./../src/auto_end_test.cpp \
	./../src/explicit_end_test.cpp \
	./../src/helper.cpp \
	./../src/listener_manager.cpp \
	./../src/result.cpp

include $(BUILD_SHARED_LIBRARY)
