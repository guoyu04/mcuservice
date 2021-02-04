/**
 * ICarSystemService.cpp
 * android系统状态模块：负责接收acc、重启等状态
 * Created by guoyu on 2019/10/27.
 */

#include<utils/Log.h>
#include <utils/String8.h>
#include <sys/wait.h>
#include "service/ICarSystemService.h"
#include "utils/ZLog.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "mcu.service.ICarSystemService"
using namespace android;

class BpCarSystemService: public BpInterface<ICarSystemService>
{
	public:
		BpCarSystemService(const sp<IBinder> &impl): BpInterface<ICarSystemService>(impl)
	{
	}
	public:
        int32_t registRemoteProxy(sp<IBinder> proxy, std::string &pckName)
        {
            Parcel data;
			data.writeInterfaceToken(ICarSystemService::getInterfaceDescriptor());
            data.writeStrongBinder( proxy );
            data.writeString16(String16("metasequoia.remote.manager"));
            Parcel reply;
            remote()->transact(BIND_REGISTER_SYS, data, &reply);
            return reply.readInt32();

        }

        int32_t unRegistRemoteProxy(sp<IBinder> proxy, std::string &pckName)
        {
            Parcel data;
			data.writeInterfaceToken(ICarSystemService::getInterfaceDescriptor());
            data.writeStrongBinder( proxy );
            data.writeString16(String16("metasequoia.remote.manager"));
            Parcel reply;
            remote()->transact(BIND_UNREGISTER_SYS, data, &reply);
            return reply.readInt32();
        }

        //发送语音
        int32_t sendSpeak(const char * data, int len)
        {
            Parcel _data;
            _data.writeInterfaceToken(ICarSystemService::getInterfaceDescriptor());
            Parcel reply;
            _data.writeInt32(len);
			for(int i=0; i< len; i ++){
			    _data.writeInt32(data[i]);
			}
            remote()->transact(BIND_SEND_SPEAK, _data, &reply);
            return reply.readInt32();
        }

       //设置当前前台应用
        int32_t setTopPackage(std::string pckName)
        {
            Parcel data;
            data.writeInterfaceToken(ICarSystemService::getInterfaceDescriptor());
             data.writeString16(String16(pckName.c_str()));
            Parcel reply;
            remote()->transact(BIND_SET_TOP_PACKAGE, data, &reply);
            return reply.readInt32();
        }

        //设置导航打开状态
        int setNavOpenState(int state)
        {
            Parcel data;
            data.writeInterfaceToken(ICarSystemService::getInterfaceDescriptor());
            data.writeInt32(state);
            Parcel reply;
            remote()->transact(BIND_SET_NAV_STATE, data, &reply);
            return reply.readInt32();
        }

        //设置蓝牙打开状态
        int setBleOpenState(int state)
        {
            Parcel data;
            data.writeInterfaceToken(ICarSystemService::getInterfaceDescriptor());
            data.writeInt32(state);
            Parcel reply;
            remote()->transact(BIND_SET_BLE_STATE, data, &reply);
            return reply.readInt32();
        }

        //设置FM打开状态
        int setFMOpenState(int state)
        {
            Parcel data;
            data.writeInterfaceToken(ICarSystemService::getInterfaceDescriptor());
            data.writeInt32(state);
            Parcel reply;
            remote()->transact(BIND_SET_FM_STATE, data, &reply);
            return reply.readInt32();
        }

        //设置人脸认证状态
        int setFaceState(int state)
        {
            return 0;
        }
        //设置评分等级
        int setGrading(int grade)
        {
            return 0;
        }
        //目的地周边优惠信息
        int setDestPerInfo(int type)
        {
            return 0;
        }
        //订单信息
        int setOrderInfo(int type)
        {
            return 0;
        }

        /**
         * 获取车辆状态
         * @return 车辆状态
         */
        int getAccState()
        {
            return 0;
        }

        /**
         * 获取剩余电量
         * @return 电量
         */
        int getBatteryPower()
        {
            return 0;
        }

        /**
         * 获取电池状态
         * @return 电池状态
         */
        int getBatteryState()
        {
            return 0;
        }

        /**
         * 获取档位状态
         * @return 档位状态
         */
        int getCarGear()
        {
            return 0;
        }

        /**
         * 获取车速
         * @return 车速
         */
        int getCarSpeed()
        {
            return 0;
        }

        /**
         * 获取转速
         * @return 转速
         */
        int getRotateSpeed()
        {
            return 0;
        }

        /**
         * 获取总里程
         * @return 总里程
         */
        int getTotalMileage()
        {
            return 0;
        }

        /**
         * 获取剩余里程
         * @return 剩余里程
         */
        int getRemMileage()
        {
            return 0;
        }

        /**
         * 获取wifi账号
         * @return wifi账号
         */
        std::string getWifiAccount()
        {
            return "";
        }

        /**
         * 获取wifi密码
         * @return wifi密码
         */
        std::string getWifiPwd()
        {
            return "";
        }

        /**
         * device id
         * @return device id
         */
        std::string getDeviceID()
        {
            return "";
        }

        /**
         * battery voltage
         * @return battery voltage
         */
        int getBatVoltage()
        {
            return 0;
        }
};

IMPLEMENT_META_INTERFACE(CarSystemService, NATIVE_SYS_SER_NAME);

status_t BnCarSystemService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{

    LOGI( LOG_TAG, "BnCarSystemService onTransact : code =  %d ", code );
	switch(code)
	{
		case BIND_REGISTER_SYS:
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : register remote service " );
				sp<IBinder> proxy = data.readStrongBinder();
                String16 sPkg16 = data.readString16();
				std::string   sPkgName = String8(sPkg16).string();
				int32_t ret = registRemoteProxy(proxy,sPkgName);
				reply->writeInt32(ret);
				return NO_ERROR;
			}

		case BIND_UNREGISTER_SYS:
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : unregister remote service " );
                sp<IBinder> proxy = data.readStrongBinder();
                String16 sPkg16 = data.readString16();
				std::string   sPkgName = String8(sPkg16).string();
				int32_t ret = unRegistRemoteProxy(proxy,sPkgName);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_SEND_SPEAK:
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
                LOGI( LOG_TAG, "ICarSystemService : on receive send speak event");
                int recLen = data.readInt32();
                char *recData = (char*)malloc(recLen);
                for(int i=0;i < recLen; i++){
                    recData[i]=(char)data.readInt32();
                }
				int32_t ret = sendSpeak(recData, recLen);
                free(recData);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_SET_TOP_PACKAGE:
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
                LOGI( LOG_TAG, "ICarSystemService : on receive set top packge event");
                String16 sPkg16 = data.readString16();
				std::string   sPkgName = String8(sPkg16).string();
				int32_t ret = setTopPackage(sPkgName);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_SET_NAV_STATE:
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
                LOGI( LOG_TAG, "ICarSystemService : on receive set navigation event");
                int32_t state = data.readInt32();
				int32_t ret = setNavOpenState(state);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_SET_BLE_STATE:
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive set ble event" );
                int32_t state = data.readInt32();
				int32_t ret = setBleOpenState(state);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_SET_FM_STATE:
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive set ble event" );
                int32_t state = data.readInt32();
				int32_t ret = setFMOpenState(state);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_ACC_STATUS://获取acc状态
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get acc state" );
				int32_t ret = getAccState();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_BAT_POWER://获取电池剩余电量
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get battery power" );
				int32_t ret = getBatteryPower();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_BAT_STATE: //获取电池状态
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get battery state" );
				int32_t ret = getBatteryState();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_CAR_GEAR://获取档位状态
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get car gear" );
				int32_t ret = getCarGear();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_CAR_SPEED://获取车速
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive geet car speed" );
				int32_t ret = getCarSpeed();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_ROTATE_SPEED://获取转速
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get rotate speed" );
				int32_t ret = getRotateSpeed();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_TOTAL_MIL://获取总里程
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get total milleage" );
				int32_t ret = getTotalMileage();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_REM_MIL://获取剩余里程
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get rem milleage" );
				int32_t ret = getRemMileage();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_WIFI_ACCOUNT://获取wifi账号
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get wifi account" );
                std::string account = getWifiAccount();
				reply->writeString16(String16(account.c_str()));
				return NO_ERROR;
			}
        case BIND_GET_WIFI_PWD://获取wifi密码
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get wifi password" );
                std::string pwd = getWifiPwd();
				reply->writeString16(String16(pwd.c_str()));
				return NO_ERROR;
			}
        case BIND_GET_BAT_VOLTAGE://获取电瓶电压
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get battery voltage" );
				int32_t ret = getBatVoltage();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_GET_DEV_ID://获取deivce id
            {
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive get tbox id" );
                std::string devID = getDeviceID();
				reply->writeString16(String16(devID.c_str()));
				return NO_ERROR;
			}
        case BIND_SET_FACE_STATE://人脸认证状态
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive set face event" );
                int32_t state = data.readInt32();
				int32_t ret = setFaceState(state);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_SET_GRADING://设置评分等级
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive set grading event" );
                int32_t state = data.readInt32();
				int32_t ret = setGrading(state);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_SET_DEST_PER_INFO://目的地周边优惠信息
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive set dest per info event" );
                int32_t state = data.readInt32();
				int32_t ret = setDestPerInfo(state);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_SET_ORDER_INFO://订单信息
			{
				CHECK_INTERFACE(ICarSystemService, data, reply);
				LOGI( LOG_TAG, "ICarSystemService : on receive set order event" );
                int32_t state = data.readInt32();
				int32_t ret = setOrderInfo(state);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
		default:
			{
				return BBinder::onTransact(code, data, reply, flags);
			}
	}
}
