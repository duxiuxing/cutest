LOCAL_PATH := $(call my-dir)

TEST_DIR := $(LOCAL_PATH)/..

include $(TEST_DIR)/ExplicitEndTest/jni/ExplicitEndTest.mk
include $(TEST_DIR)/money/jni/money.mk
include $(TEST_DIR)/sample1/jni/sample1.mk
include $(TEST_DIR)/sample2/jni/sample2.mk
include $(TEST_DIR)/sample3/jni/sample3.mk
include $(TEST_DIR)/sample4/jni/sample4.mk
include $(TEST_DIR)/sample5/jni/sample5.mk
include $(TEST_DIR)/simple/jni/simple.mk
