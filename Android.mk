LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=src/main/Main.cpp \
        src/utils/Lock.cpp \
        src/utils/ZLog.cpp \
		src/utils/Base64.cpp \
        src/utils/Utils.cpp \
		src/service/RemoteServiceList.cpp \
		src/service/ICarSystemService.cpp \
        src/service/CarSystemService.cpp \
		src/service/IMcuHardService.cpp \
        src/service/McuHardService.cpp \
        src/common/GetPkgInfo.cpp \
        src/common/CircularQueue.cpp \
        src/common/CrcCalc.cpp \
        src/common/Common.cpp \
        src/common/TThread.cpp \
        src/common/ThreadPool.cpp \
        src/common/MsgData.cpp \
        src/device/BaseDevice.cpp \
        src/device/BaseUsbDevice.cpp \
        src/device/CarMcuDevice.cpp \
        src/device/SysDevice.cpp \
        src/device/McuHDDevice.cpp \
        src/device/McuIODevice.cpp \
		src/serial/UsbSerial.cpp \
        src/decoupler/Decoupler.cpp \
        src/decoupler/UsbDecoupler.cpp \
        src/decoupler/DecouplerData.cpp \
        src/decoupler/DecouplerManager.cpp \
        src/mcu/UsbMcuCenter.cpp \
        

LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS -Wl,-Map=test.map -g -Wno-unused-parameter -Wno-unused-variable

LOCAL_CONLYFLAGS := -std=c11

LOCAL_SHARED_LIBRARIES := libcutils \
		libsysutils \
        libutils \
        libbinder \
        libui \
        libgui \
        liblog

#LOCAL_MULTILIB := both
#LOCAL_MODULE_STEM_32 := mcuservice
#LOCAL_MODULE_STEM_64 := mcuservice64
LOCAL_LDLIBS := -llog
LOCAL_MODULE := mcuservice

LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES += external/stlport/stlport bionic
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES += libstlport libstdc++
#ifeq ($(TARGET_IS_64_BIT),true)
#     $(LOCAL_MODULE): $(LOCAL_MODULE)$(TARGET_2ND_ARCH_MODULE_SUFFIX)
#endif

include $(BUILD_EXECUTABLE)

