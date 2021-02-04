/**
 * ICarSystemService.h
 * android系统状态模块：负责接收acc、重启等状态
 * Created by guoyu on 2019/10/27.
 */

#ifndef _I_CAR_SYS_SERVICES_H_
#define _I_CAR_SYS_SERVICES_H_

#include <list>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <string>

#define NATIVE_SYS_SER_NAME "metasequoia.mcu.CarSystemService"
#define REMOTE_SYS_SER_NAME "metasequoia.mcu.ICarSystemManager"


using namespace android;
enum
{
    BIND_REGISTER_SYS = IBinder::FIRST_CALL_TRANSACTION,
    BIND_UNREGISTER_SYS,
    BIND_SEND_SPEAK, //发送语音
    BIND_SET_TOP_PACKAGE, //设置当前前台应用
    BIND_SET_NAV_STATE, //设置导航打开状态
    BIND_SET_BLE_STATE, //设置蓝牙打开状态
    BIND_SET_FM_STATE, //设置FM打开状态
    BIND_NOTIFY_ACC_STATUS, //acc状态通知事件
    BIND_NOTIFY_BAT_POWER, //电池剩余电量
    BIND_NOTIFY_BAT_STATE, //电池状态
    BIND_NOTIFY_CAR_GEAR,//档位状态
    BIND_NOTIFY_CAR_SPEED,//车速
    BIND_NOTIFY_ROTATE_SPEED,//转速
    BIND_NOTIFY_TOTAL_MIL,//总里程
    BIND_NOTIFY_REM_MIL,//剩余里程
    BIND_NOTIFY_WIFI_ACCOUNT,//wifi账号
    BIND_NOTIFY_WIFI_PWD,//wifi密码

    BIND_GET_ACC_STATUS, //获取acc状态
    BIND_GET_BAT_POWER, //获取电池剩余电量
    BIND_GET_BAT_STATE, //获取电池状态
    BIND_GET_CAR_GEAR,//获取档位状态
    BIND_GET_CAR_SPEED,//获取车速
    BIND_GET_ROTATE_SPEED,//获取转速
    BIND_GET_TOTAL_MIL,//获取总里程
    BIND_GET_REM_MIL,//获取剩余里程
    BIND_GET_WIFI_ACCOUNT,//获取wifi账号
    BIND_GET_WIFI_PWD,//获取wifi密码
    
    BIND_NOTIFY_KEY_EVENT,//key事件通知
    
    BIND_SET_FACE_STATE,//人脸认证状态
    BIND_SET_GRADING,//设置评分等级
    BIND_SET_DEST_PER_INFO,//目的地周边优惠信息
    BIND_SET_ORDER_INFO,//订单信息

    BIND_NOTIFY_DEV_ID,//Tbox ID
    BIND_GET_DEV_ID, //获取Tbox ID

    BIND_NOTIFY_BAT_VOLTAGE, //小电瓶电压
    BIND_GET_BAT_VOLTAGE, //获取小电瓶电压
};

class ICarSystemService: public IInterface
{
    public:
        DECLARE_META_INTERFACE(CarSystemService);
    public:
        virtual int32_t registRemoteProxy(sp<IBinder> proxy, std::string &pckName) = 0;
        virtual int32_t unRegistRemoteProxy(sp<IBinder> proxy, std::string &pckName) = 0;
        //发送语音
        virtual int32_t sendSpeak(const char * data, int len) = 0;
        //设置当前前台应用
        virtual int32_t setTopPackage(std::string pckName) = 0;
        //设置导航打开状态
        virtual int setNavOpenState(int state) = 0;
        //设置蓝牙打开状态
        virtual int setBleOpenState(int state) = 0;
        //设置FM打开状态
        virtual int setFMOpenState(int state) = 0;

        //设置人脸认证状态
        virtual int setFaceState(int state) = 0;
        //设置评分等级
        virtual int setGrading(int grade) = 0;
        //目的地周边优惠信息
        virtual int setDestPerInfo(int type) = 0;
        //订单信息
        virtual int setOrderInfo(int type) = 0;

        virtual int getAccState() = 0;//获取车辆状态
        virtual int getBatteryPower() = 0;//获取剩余电量
        virtual int getBatteryState() = 0;//获取电池状态
        virtual int getCarGear() = 0;//获取档位状态
        virtual int getCarSpeed() = 0;//获取车速
        virtual int getRotateSpeed() = 0;//获取转速
        virtual int getTotalMileage() = 0;//获取总里程
        virtual int getRemMileage() = 0;//获取剩余里程
        virtual std::string getWifiAccount() = 0;//获取wifi账号
        virtual std::string getWifiPwd() = 0;//获取wifi密码
        virtual std::string getDeviceID() = 0;//获取device id
        virtual int getBatVoltage() = 0;//获取小电瓶电压
};

class BnCarSystemService: public BnInterface<ICarSystemService>
{
	public:
		virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags = 0);
};
#endif
