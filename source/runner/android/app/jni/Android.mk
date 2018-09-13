LOCAL_PATH := $(call my-dir)

TEST_DIR := $(LOCAL_PATH)/../../../../test

# libcutest.so
include $(LOCAL_PATH)/../../../../third_party/cutest/jni/Android.mk

# *.so
include $(TEST_DIR)/jni/test.mk
