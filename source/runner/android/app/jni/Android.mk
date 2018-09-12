LOCAL_PATH := $(call my-dir)

TESTS_DIR := $(LOCAL_PATH)/../../../../tests

# 先编译libcutest.so
include $(LOCAL_PATH)/../../../../third_party/cutest/jni/Android.mk

# 再编译测试用例的so
# include $(TESTS_DIR)/cppunit/jni/cppunit_samples.mk
# include $(TESTS_DIR)/gtest/sample1/jni/sample1.mk
# include $(TESTS_DIR)/gtest/sample2/jni/sample2.mk
