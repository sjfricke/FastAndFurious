LOCAL_PATH := $(call my-dir)
PROJECT_ROOT:= $(call my-dir)/../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE    := libFaF
LOCAL_CFLAGS    := -Werror -Wno-write-strings -std=c++11
LOCAL_SRC_FILES := native-lib.cpp
LOCAL_LDLIBS    := -llog -landroid -lcamera2ndk -lmediandk
include $(BUILD_SHARED_LIBRARY)