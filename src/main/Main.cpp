/**
 * main.cpp
 * 主程序
 * Created by guoyu on 2019/8/27.
 */

#define LOG_TAG "McuMain"
//#define LOG_NDEBUG 0
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <binder/Parcel.h>
#include <stdlib.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <signal.h>
#include <sys/wait.h>
#include <string>
#include <private/binder/binder_module.h>
#include "utils/ZLog.h"
#include "service/CarSystemService.h"
#include "service/McuHardService.h"
#include "mcu/UsbMcuCenter.h"
#include "decoupler/DecouplerManager.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "mcu.main"


using namespace android;

int main(int argc, char **argv)
{
    sp<ProcessState> proc(ProcessState::self());
    DecouplerManager::getInstance()->init();
    UsbMcuCenter::getInstance()->init();

    CarSystemService::getInstance()->init();
    McuHardService::getInstance()->init();

    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
