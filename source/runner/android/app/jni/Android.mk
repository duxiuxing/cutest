LOCAL_PATH := $(call my-dir)

TEST_DIR := $(LOCAL_PATH)/../../../../test

# libcutest.so
include $(LOCAL_PATH)/../../../../third_party/cutest/jni/Android.mk

# *.so
# include $(LOCAL_PATH)/../../../../test/jni/test.mk
include $(TEST_DIR)/money/jni/money.mk
include $(TEST_DIR)/simple/jni/simple.mk
include $(TEST_DIR)/ExplicitEndTest/jni/ExplicitEndTest.mk
include $(TEST_DIR)/sample1/jni/sample1.mk
include $(TEST_DIR)/sample2/jni/sample2.mk
