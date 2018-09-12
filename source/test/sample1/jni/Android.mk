LOCAL_PATH := $(call my-dir)

# libcutest.so
include $(LOCAL_PATH)/../../../third_party/cutest/jni/Android.mk

# libsample1.so
include sample1.mk
