LOCAL_PATH := $(call my-dir)

# libcutest.so
include $(LOCAL_PATH)/../../../third_party/cutest/jni/Android.mk

# libsample2.so
include sample2.mk
