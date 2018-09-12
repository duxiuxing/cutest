LOCAL_PATH := $(call my-dir)

TEST_DIR := $(LOCAL_PATH)/..

# libcutest.so
include $(LOCAL_PATH)/../../third_party/cutest/jni/Android.mk

# *.so
include test.mk
