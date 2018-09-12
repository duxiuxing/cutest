LOCAL_PATH := $(call my-dir)

TEST_DIR := $(LOCAL_PATH)/../../../../test

# libcutest.so
include $(LOCAL_PATH)/../../../../third_party/cutest/jni/Android.mk

# *.so
# include $(TEST_DIR)/hierarchy/jni/Android.mk
include $(TEST_DIR)/money/jni/money.mk
include $(TEST_DIR)/simple/jni/simple.mk
include $(TEST_DIR)/ExplicitEndTest/jni/ExplicitEndTest.mk
