LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := cutest

LOCAL_CPPFLAGS += \
    -frtti -fexceptions

LOCAL_LDLIBS := -llog

#cppunit begin
CPPUNIT_INC_DIR := \
    $(LOCAL_PATH)/../../cppunit/include \
    $(LOCAL_PATH)/../../cppunit/src/cppunit
#cppunit end

#gtest begin
GTEST_INC_DIR := \
    $(LOCAL_PATH)/../../googletest \
    $(LOCAL_PATH)/../../googletest/include
#gtest end

#gmock begin
GMOCK_INC_DIR := \
    $(LOCAL_PATH)/../../googlemock \
    $(LOCAL_PATH)/../../googlemock/include
#gtest end

#cutest begin
CUTEST_INC_DIR := \
    $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/../src
#cutest end

LOCAL_C_INCLUDES := \
    $(CPPUNIT_INC_DIR) \
    $(GTEST_INC_DIR) \
	$(GMOCK_INC_DIR) \
	$(CUTEST_INC_DIR)

LOCAL_SRC_FILES := \
    ./../../cppunit/src/cppunit/cppunit-all.cpp \
    ./../../googletest/src/gtest-all.cc \
	./../../googlemock/src/gmock-all.cc \
	./../src/AutoEndTest.cpp \
	./../src/ExplicitEndTest.cpp \
	./../src/Helper.cpp \
	./../src/ProgressListenerManager.cpp \
	./../src/Result.cpp \
	./../src/RunnerBase.cpp \
	./../src/android/DecoratorImpl.cpp \
	./../src/android/JClassManager.cpp \
	./../src/android/JniEnv.cpp \
	./../src/android/JniHelp.cpp \
	./../src/android/Logger.cpp \
	./../src/android/ProgressListenerJni.cpp \
	./../src/android/RunnerImpl.cpp \
	./../src/android/SynchronizationObjectImpl.cpp

include $(BUILD_SHARED_LIBRARY)
