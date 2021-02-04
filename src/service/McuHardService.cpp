/**
 * McuHardService.cpp
 * MCU信息接口
 * Created by guoyu on 2019/11/21.
 */
#include <utils/Log.h>
#include <utils/String8.h>
#include <binder/IServiceManager.h>
#include <string>
#include <pthread.h>
#include "service/McuHardService.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"
#include "device/McuHDDevice.h"
#include "common/GetPkgInfo.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "mcu.service.McuHardService"


McuHardService * McuHardService::s_ptrService;
android::sp<RemoteServiceList> McuHardService::s_vBinders;
ZMutex McuHardService::s_ServiceLock;

//death notifier
//sp<McuHardService::DeathNotifier> McuHardService::sDeathNotifier;

/**
 * 构造函数
 */
McuHardService::McuHardService()
{
    s_vBinders = new RemoteServiceList();
}

/**
 * 析构函数
 */
McuHardService::~McuHardService()
{
    if(m_ptrDev != NULL)
    {
        delete m_ptrDev;
    }
}

/**
 * 初始化
 */
void McuHardService::init()
{
    LOGD( LOG_TAG, "<init mcu service>\n");
    defaultServiceManager()->addService(String16(NATIVE_MCU_SER_NAME), s_ptrService);
    m_ptrDev = new McuHDDevice();
    m_ptrDev->setMcuUpdateDevListener(this);
    //启动线程池
    setThreadCount(2);
    start();
    //TODO
    //m_ptrDev->requestMcuVersion();
    //m_ptrDev->requestMcuUpdate("/sdcard/mogo_app.bin");
}

/**
 * 获取服务单例
 * @return 服务句柄
 */
McuHardService * McuHardService::getInstance()
{
    if(!s_ptrService)
    {
       ZMutex::Autolock lock(s_ServiceLock);
       s_ptrService = new McuHardService();
    }
    return s_ptrService;
}

/**
 * 注册代理
 * @param proxy 代理句柄
 * @param pckname 代理包名
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int32_t McuHardService::registRemoteProxy(sp<IBinder> proxy, std::string &pckName)
{
    ZMutex::Autolock lock(s_ServiceLock);
    std::string remotePck = GetPkgInfo::getPkgNameFromUid(Utils::getCallingUid());
    s_vBinders->addList(proxy);
    LOGI( LOG_TAG, "McuHardService : registRemoteProxy remote proxy %s, %p, pid[%s]", pckName.c_str(), proxy.get(), remotePck.c_str());
    return RTN_SUCCESS;
}

/**
 * 注销代理
 * @param proxy 代理句柄
 * @param pckname 代理包名
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int32_t McuHardService::unRegistRemoteProxy(sp<IBinder> proxy, std::string &pckName __unused)
{
    s_vBinders->removeList(proxy);
    LOGI( LOG_TAG, "find no proxy pointer value = [%p]\n", proxy.get());
    return RTN_SUCCESS;
}


/**
 * 消息运行
 */
void McuHardService::run(TThread * thread)
{
    LOGD(LOG_TAG,"run");
    while(1)
    {
        MsgData* data =  m_MsgQueue.getMsg();

        string sendData1 = data->getData1();
        string sendData2 = data->getData2();
        android::Parcel parcel;
	    parcel.writeInterfaceToken(String16(REMOTE_MCU_SER_NAME));
	    parcel.writeInt32(data->getArg1());
        parcel.writeString16(String16(sendData1.c_str()));
        parcel.writeString16(String16(sendData2.c_str()));
        LOGI( LOG_TAG, "set data to remote: code [%d], arg1[%d], data1[%s], data2[%s]", data->getWhat(), data->getArg1(), sendData1.c_str(), sendData2.c_str());
        s_vBinders->sendData2Remote(data->getWhat(), parcel);
        //处理完删除
        delete data;
    }
}

/**
 * 请求MCU版本信息
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int32_t McuHardService::requestMcuVersion()
{
    LOGI( LOG_TAG, "request mcu version");
    m_ptrDev->requestMcuVersion();
    return RTN_SUCCESS;
}

/**
 * 请求MCU版本更新
 * @param binPath bin文件路径
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int32_t McuHardService::requestMcuUpdate(std::string binPath)
{
    LOGI( LOG_TAG, "request mcu update");
    int iRet = m_ptrDev->requestMcuUpdate(binPath);
    if(iRet == RTN_FAIL)
    {
        onRecMcuUpdateState(RTN_FAIL);
    }
    return RTN_SUCCESS;
}

/**
 * 接收到mcu版本请求结果消息
 */
void McuHardService::onRecMcuVersion(const std::string soft, const std::string hard)
{
    LOGD(LOG_TAG,"onRecMcuVersion: soft[%s], hard[%s]", soft.c_str(), hard.c_str());
    MsgData * data = new MsgData(BIND_NOTIFY_MCU_VER);
    data->setArg1(0);
    data->setData1(soft);
    data->setData2(hard);
    m_MsgQueue.putMsg(data);
}

/**
 * MCU更新状态
 */
void McuHardService::onRecMcuUpdateState(int status)
{
    LOGD(LOG_TAG,"onRecMcuUpdateState: status[%d]", status);
    MsgData * data = new MsgData(BIND_NOTIFY_MCU_UPDATE_STATE);
    data->setArg1(status);
    data->setData1("");
    data->setData2("");
    m_MsgQueue.putMsg(data);
}
