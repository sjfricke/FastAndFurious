LOCAL_PATH := $(call my-dir)
LIBS_PATH  := $(call my-dir)/../../../libs
INCLUDE_PATH  := $(call my-dir)/../include

include $(CLEAR_VARS)

LOCAL_MODULE    := libFaF
LOCAL_CFLAGS    := -Werror -Wno-write-strings -std=c++11
LOCAL_C_INCLUDES := $(INCLUDE_PATH)/CL/opencl20

LOCAL_SRC_FILES := native-lib.cpp \
                   FastAndFurious.cpp \
                   ImageReader.cpp \
                   NativeCamera.cpp \
                   GaussianBlur.cpp

LOCAL_LDLIBS    := -llog -landroid -lcamera2ndk -lmediandk $(LIBS_PATH)/libOpenCL.so
include $(BUILD_SHARED_LIBRARY)