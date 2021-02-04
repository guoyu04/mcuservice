/**
 * IMcuHardService.h
 * MCU信息接口
 * Created by guoyu on 2019/11/21.
 */

#ifndef _I_CAR_MCU_SERVICES_H_
#define _I_CAR_MCU_SERVICES_H_

#include <list>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <string>

#define NATIVE_MCU_SER_NAME "metasequoia.mcu.CarMcuService"
#define REMOTE_MCU_SER_NAME "metasequoia.mcu.ICarMcuManager"


using namespace android;
enum
{
    BIND_REGISTER_MCU = IBinder::FIRST_CALL_TRANSACTION,
    BIND_UNREGISTER_MCU,
    BIND_REQ_MCU_VER, //请求MCU版本信息
    BIND_REQ_MCU_UPDATE, //请求MCU版本更新
    BIND_NOTIFY_MCU_VER, //回复MCU版本
    BIND_NOTIFY_MCU_UPDATE_STATE, //回复MCU更新状态
};

class IMcuHardService: public IInterface
{
    public:
        DECLARE_META_INTERFACE(McuHardService);
    public:
        virtual int32_t registRemoteProxy(sp<IBinder> proxy, std::string &pckName) = 0;
        virtual int32_t unRegistRemoteProxy(sp<IBinder> proxy, std::string &pckName) = 0;
        //请求MCU版本信息
        virtual int32_t requestMcuVersion() = 0;
        //请求MCU版本更新
        virtual int32_t requestMcuUpdate(std::string binPath) = 0;
};

class BnMcuHardService: public BnInterface<IMcuHardService>
{
	public:
		virtual status_t onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags = 0);
};
#endif
