aOCAL_PATH := $(call my-dir)

# libcutest.so
include $(LOCAL_PATH)/../../../third_party/cutest/jni/Android.mk

# libExplicitEndTest.so
include explicit_end_test.mk
