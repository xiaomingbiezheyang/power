#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

# power_example executable
# ========================================================

include $(CLEAR_VARS)
LOCAL_MODULE := qihanpower
LOCAL_C_INCLUDES :=  $(LOCAL_PATH)/include
LOCAL_SRC_FILES := \
  qihanpower.cpp \
  getevent.cpp \
  utils.cpp \

LOCAL_SHARED_LIBRARIES := \
    libcutils 

ifeq "$(findstring octopus_qh106,$(TARGET_PRODUCT))" "octopus_qh106"
  LOCAL_CFLAGS += -DOCTOPUS_QH106_PRODUCT
endif
 
LOCAL_MODULE_TAGS := optional
ALL_DEFAULT_INSTALLED_MODULES += $(LOCAL_MODULE)
    
include $(BUILD_EXECUTABLE)
