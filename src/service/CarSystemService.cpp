/**
 * CarSystemService.cpp
 * android系统状态模块：负责接收acc、重启等状态
 * Created by guoyu on 2019/10/27.
 */
#include <utils/Log.h>
#include <utils/String8.h>
#include <binder/IServiceManager.h>
#include <string>
#include <pthread.h>
#include "service/CarSystemService.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"
#include "device/SysDevice.h"
#include "common/GetPkgInfo.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "mcu.service.CarSystemService"

#define ACC_POWER_PATH "/sys/class/power_supply/Mains/online"

#define VEHICLE_ILL_DEV         "ill_signal"                //大灯设备信号
#define VEHICLE_BACK_DEV        "back_signal"               //倒车设备信号

CarSystemService * CarSystemService::s_ptrService;
android::sp<RemoteServiceList> CarSystemService::s_vBinders;
ZMutex CarSystemService::s_ServiceLock;

//death notifier
//sp<CarSystemService::DeathNotifier> CarSystemService::sDeathNotifier;

/**
 * 构造函数
 */
CarSystemService::CarSystemService()
{
    s_vBinders = new RemoteServiceList();
}

/**
 * 析构函数
 */
CarSystemService::~CarSystemService()
{
    if(m_ptrSysDev != NULL)
    {
        //m_ptrSysDev->closeDevice();
        delete m_ptrSysDev;
    }

    /*if(m_ptrBtnDev != NULL)
    {
        delete m_ptrBtnDev;
    }*/
}

/**
 * 初始化
 */
void CarSystemService::init()
{
    LOGD( LOG_TAG, "<init power service>\n");
    defaultServiceManager()->addService(String16(NATIVE_SYS_SER_NAME), s_ptrService);
    m_ptrSysDev = new SysDevice();
    m_ptrIODev = new McuIODevice();
    m_ptrIODev->setMcuIODevListener(this);

    m_ptrMcuDev = new CarMcuDevice();
    m_ptrMcuDev->setCarMcuDevListener(this);
    //启动线程池
    setThreadCount(2);
    start();
}

/**
 * 获取服务单例
 * @return 服务句柄
 */
CarSystemService * CarSystemService::getInstance()
{
    if(!s_ptrService)
    {
       ZMutex::Autolock lock(s_ServiceLock);
       s_ptrService = new CarSystemService();
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
int32_t CarSystemService::registRemoteProxy(sp<IBinder> proxy, std::string &pckName)
{
    ZMutex::Autolock lock(s_ServiceLock);
    std::string remotePck = GetPkgInfo::getPkgNameFromUid(Utils::getCallingUid());
    s_vBinders->addList(proxy);
    LOGI( LOG_TAG, "CarSystemService : registRemoteProxy remote proxy %s, %p, pid[%s]", pckName.c_str(), proxy.get(), remotePck.c_str());
    return RTN_SUCCESS;
}

/**
 * 注销代理
 * @param proxy 代理句柄
 * @param pckname 代理包名
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int32_t CarSystemService::unRegistRemoteProxy(sp<IBinder> proxy, std::string &pckName __unused)
{
    s_vBinders->removeList(proxy);
    LOGI( LOG_TAG, "find no proxy pointer value = [%p]\n", proxy.get());
    return RTN_SUCCESS;
}

/**
 * 添加消息到消息队列
 */
void CarSystemService::addMessage(int code, int status)
{
    MsgData * data = new MsgData(code);
    data->setArg1(status);
    m_MsgQueue.putMsg(data);
}

/**
 * 消息运行
 */
void CarSystemService::run(TThread * thread)
{
    LOGD(LOG_TAG,"run");
    while(1)
    {
        MsgData* data =  m_MsgQueue.getMsg();
        //string sendData = data->getData();
        android::Parcel parcel;
	    parcel.writeInterfaceToken(String16(REMOTE_SYS_SER_NAME));
	    parcel.writeInt32(data->getArg1());
        if(data->haveData1())
        {
            parcel.writeString16(String16(data->getData1().c_str()));
        }
        parcel.writeInt32(data->getArg2());
        LOGI( LOG_TAG, "set data to remote: code [%d], status[%d]", data->getWhat(), data->getArg1());
        s_vBinders->sendData2Remote(data->getWhat(), parcel);
        //处理完删除
        delete data;
    }
}

/**
 * 发送语音
 * @param  data[IN] 语音数据
 * @param  len[IN] 语音数据的长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
//
int32_t CarSystemService::sendSpeak(const char * data, int len)
{
    LOGI( LOG_TAG, "sendSpeak:%d",len);
    DebugBuffer(LOG_TAG, "send data:",data,len);
    return m_ptrSysDev->sendSpeak(data, len);
}

/**
 * 设置当前前台应用
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int32_t CarSystemService::setTopPackage(std::string pckName)
{
    LOGI( LOG_TAG, "set top package name");
    return m_ptrSysDev->setTopPackage(pckName);
}

/**
 * 设置导航打开状态
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int CarSystemService::setNavOpenState(int state)
{
    return m_ptrSysDev->setNavOpenState(state);
}

/**
 * 设置蓝牙打开状态
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int CarSystemService::setBleOpenState(int state)
{
    return m_ptrSysDev->setBleOpenState(state);
}

/**
 * 设置FM打开状态
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int CarSystemService::setFMOpenState(int state)
{
    return m_ptrSysDev->setFMOpenState(state);
}


/**
 * 设置人脸认证状态
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int CarSystemService::setFaceState(int state)
{
    return m_ptrSysDev->setFaceState(state);;
}

/**
 * 设置评分等级
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int CarSystemService::setGrading(int grade)
{
    return m_ptrSysDev->setGrading(grade);
}

/**
 * 目的地周边优惠信息
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int CarSystemService::setDestPerInfo(int type)
{
    return m_ptrSysDev->setDestPerInfo(type);
}
/**
 * 订单信息
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int CarSystemService::setOrderInfo(int type)
{
    return m_ptrSysDev->setOrderInfo(type);
}

/**
 * 接收到车辆状态
 * @param [IN] state 状态
 */
void CarSystemService::onRecAccState(char state)
{
    LOGD(LOG_TAG,"acc: %d", state);
    addMessage(BIND_NOTIFY_ACC_STATUS, (int)state);
}

/**
 * 接收到剩余电量
 * @param [IN] battery 电量
 */
void CarSystemService::onRecBatPowerChange(char battery)
{
    LOGD(LOG_TAG,"battery: %d", battery);
    addMessage(BIND_NOTIFY_BAT_POWER, (int)battery);
}

/**
 * 接收到电池状态
 * @param [IN] state 状态
 */
void CarSystemService::onRecBatState(char state)
{
    addMessage(BIND_NOTIFY_BAT_STATE, (int)state);
}

/**
 * 接收到档位状态
 * @param [IN] gear 状态
 */
void CarSystemService::onRecCarGear(char gear)
{
    addMessage(BIND_NOTIFY_CAR_GEAR, (int)gear);
}

/**
 * 接收到车速
 * @param [IN] speed 车速 
 */
void CarSystemService::onRecCarSpeed(int speed)
{
    addMessage(BIND_NOTIFY_CAR_SPEED, speed);
}

/**
 * 接收到转速
 * @param [IN] speed  转速
 */
void CarSystemService::onRecRotateSpeed(int speed)
{
    addMessage(BIND_NOTIFY_ROTATE_SPEED, speed);
}

/**
 * 接收到总里程
 * @param [IN] mileage 总里程
 */
void CarSystemService::onRecTotalMileage(int mileage)
{
    addMessage(BIND_NOTIFY_TOTAL_MIL, mileage);
}

/**
 * 接收到剩余里程
 * @param [IN] mileage 剩余里程
 */
void CarSystemService::onRecRemMileage(int mileage)
{
    addMessage(BIND_NOTIFY_REM_MIL, mileage);
}

/**
 * wifi账号
 * @param [IN] account wifi账号
 */
void CarSystemService::onRecWifiAccount(std::string account)
{
    MsgData * data = new MsgData(BIND_NOTIFY_WIFI_ACCOUNT);
    data->setData1(account);
    m_MsgQueue.putMsg(data);
}

/**
 * 接收到wifi密码
 * @param [IN] pwd wifi密码
 */
void CarSystemService::onRecWifiPwd(std::string pwd)
{
    MsgData * data = new MsgData(BIND_NOTIFY_WIFI_PWD);
    data->setData1(pwd);
    m_MsgQueue.putMsg(data);
}

/**
 * Tbox ID
 * @param [IN] devID device id
 */
void CarSystemService::onRecDevID(std::string devID)
{
    MsgData * data = new MsgData(BIND_NOTIFY_DEV_ID);
    data->setData1(devID);
    m_MsgQueue.putMsg(data);
}

/**
 * 接收到小电瓶电压
 * @param [IN] voltage 车速 
 */
void CarSystemService::onRecBatVoltage(int voltage)
{
    addMessage(BIND_NOTIFY_BAT_VOLTAGE, voltage);
}

/**
 * 获取车辆状态
 * @return 车辆状态
 */
int CarSystemService::getAccState()
{
    return m_ptrMcuDev->getAccState();
}

/**
 * 获取剩余电量
 * @return 电量
 */
int CarSystemService::getBatteryPower()
{
    return m_ptrMcuDev->getBatteryPower();
}

/**
 * 获取电池状态
 * @return 电池状态
 */
int CarSystemService::getBatteryState()
{
    return m_ptrMcuDev->getBatteryState();
}

/**
 * 获取档位状态
 * @return 档位状态
 */
int CarSystemService::getCarGear()
{
    return m_ptrMcuDev->getCarGear();
}

/**
 * 获取车速
 * @return 车速
 */
int CarSystemService::getCarSpeed()
{
    return m_ptrMcuDev->getCarSpeed();
}

/**
 * 获取转速
 * @return 转速
 */
int CarSystemService::getRotateSpeed()
{
    return m_ptrMcuDev->getRotateSpeed();
}

/**
 * 获取总里程
 * @return 总里程
 */
int CarSystemService::getTotalMileage()
{
    return m_ptrMcuDev->getTotalMileage();
}

/**
 * 获取剩余里程
 * @return 剩余里程
 */
int CarSystemService::getRemMileage()
{
    return m_ptrMcuDev->getRemMileage();
}

/**
 * 获取wifi账号
 * @return wifi账号
 */
std::string CarSystemService::getWifiAccount()
{
    return m_ptrMcuDev->getWifiAccount();
}

/**
 * 获取wifi密码
 * @return wifi密码
 */
std::string CarSystemService::getWifiPwd()
{
    return m_ptrMcuDev->getWifiPwd();
}

/**
 * device id
 * @return device id
 */
std::string CarSystemService::getDeviceID()
{
    return m_ptrMcuDev->getDeviceID();
}

/**
 * 获取小电瓶电压
 * @return 电压
 */
int CarSystemService::getBatVoltage()
{
	return m_ptrMcuDev->getBatVoltage();
}

/**
 * io事件通知
 */
void CarSystemService::onRecKeyEvent(char key, char state)
{
    LOGD(LOG_TAG,"onRecKeyEvent: %d--%d", key, state);
    m_ptrSysDev->setSOSEvent(key, state);
    MsgData * data = new MsgData(BIND_NOTIFY_KEY_EVENT);
    data->setArg1(key);
    data->setArg2(state);
    m_MsgQueue.putMsg(data);
}
