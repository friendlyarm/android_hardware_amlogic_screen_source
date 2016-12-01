# Copyright (C) 2013 Amlogic
#
#

LOCAL_PATH := $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# /system/lib/hw/screen_source.$(TARGET_DEVICE).so
include $(CLEAR_VARS)

MESON_GRALLOC_DIR ?= hardware/amlogic/gralloc

LOCAL_SRC_FILES := \
    aml_screen.cpp v4l2_vdin.cpp

LOCAL_C_INCLUDES += \
    frameworks/native/include/utils \
    frameworks/native/include/android \
    system/core/include/utils \
    system/core/libion/include \
    system/core/libion/kernel-headers \
    $(MESON_GRALLOC_DIR)

LOCAL_CFLAGS := -DLOG_TAG=\"screen_source\"

LOCAL_SHARED_LIBRARIES := libutils liblog libui

LOCAL_MODULE := screen_source.$(TARGET_DEVICE)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
