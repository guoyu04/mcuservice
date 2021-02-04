/**
 * IMcuHardService.cpp
 * MCU信息接口
 * Created by guoyu on 2019/11/21.
 */

#include<utils/Log.h>
#include <utils/String8.h>
#include <sys/wait.h>
#include "service/IMcuHardService.h"
#include "utils/ZLog.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "mcu.service.IMcuHardService"
using namespace android;

class BpMcuHardService: public BpInterface<IMcuHardService>
{
	public:
		BpMcuHardService(const sp<IBinder> &impl): BpInterface<IMcuHardService>(impl)
	{
	}
	public:
        int32_t registRemoteProxy(sp<IBinder> proxy, std::string &pckName)
        {
            Parcel data;
			data.writeInterfaceToken(IMcuHardService::getInterfaceDescriptor());
            data.writeStrongBinder( proxy );
            data.writeString16(String16("metasequoia.remote.manager"));
            Parcel reply;
            remote()->transact(BIND_REGISTER_MCU, data, &reply);
            return reply.readInt32();

        }

        int32_t unRegistRemoteProxy(sp<IBinder> proxy, std::string &pckName)
        {
            Parcel data;
			data.writeInterfaceToken(IMcuHardService::getInterfaceDescriptor());
            data.writeStrongBinder( proxy );
            data.writeString16(String16("metasequoia.remote.manager"));
            Parcel reply;
            remote()->transact(BIND_UNREGISTER_MCU, data, &reply);
            return reply.readInt32();
        }

        //请求MCU版本信息
        int32_t requestMcuVersion()
        {
            Parcel data;
            data.writeInterfaceToken(IMcuHardService::getInterfaceDescriptor());
            Parcel reply;
            remote()->transact(BIND_REQ_MCU_VER, data, &reply);
            return reply.readInt32();
        }

        int32_t requestMcuUpdate(std::string binPath)
        {
            Parcel data;
            data.writeInterfaceToken(IMcuHardService::getInterfaceDescriptor());
            data.writeString16(String16(binPath.c_str()));
            Parcel reply;
            remote()->transact(BIND_REQ_MCU_UPDATE, data, &reply);
            return reply.readInt32();
        }
};

IMPLEMENT_META_INTERFACE(McuHardService, NATIVE_MCU_SER_NAME);

status_t BnMcuHardService::onTransact(uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{

	LOGI( LOG_TAG, "BnMcuHardService onTransact : code =  %d ", code );
	switch(code)
	{
		case BIND_REGISTER_MCU:
			{
				CHECK_INTERFACE(IMcuHardService, data, reply);
				LOGI( LOG_TAG, "IMcuHardService : register remote service " );
				sp<IBinder> proxy = data.readStrongBinder();
                String16 sPkg16 = data.readString16();
				std::string   sPkgName = String8(sPkg16).string();
				int32_t ret = registRemoteProxy(proxy,sPkgName);
				reply->writeInt32(ret);
				return NO_ERROR;
			}

		case BIND_UNREGISTER_MCU:
			{
				CHECK_INTERFACE(IMcuHardService, data, reply);
				LOGI( LOG_TAG, "IMcuHardService : unregister remote service " );
                sp<IBinder> proxy = data.readStrongBinder();
                String16 sPkg16 = data.readString16();
				std::string   sPkgName = String8(sPkg16).string();
				int32_t ret = unRegistRemoteProxy(proxy,sPkgName);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_REQ_MCU_VER:
			{
				CHECK_INTERFACE(IMcuHardService, data, reply);
                LOGI( LOG_TAG, "IMcuHardService : on receive request mcu version event");
				int32_t ret = requestMcuVersion();
				reply->writeInt32(ret);
				return NO_ERROR;
			}
        case BIND_REQ_MCU_UPDATE:
			{
				CHECK_INTERFACE(IMcuHardService, data, reply);
                LOGI( LOG_TAG, "IMcuHardService : on receive request mcu update event");
                String16 sPath16 = data.readString16();
				std::string sPath = String8(sPath16).string();
				int32_t ret = requestMcuUpdate(sPath);
				reply->writeInt32(ret);
				return NO_ERROR;
			}
		default:
			{
				return BBinder::onTransact(code, data, reply, flags);
			}
	}
}
