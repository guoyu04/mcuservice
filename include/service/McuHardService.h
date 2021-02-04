/**
 * McuHardService.h
 * MCU信息接口 
 * Created by guoyu on 2019/11/21.
 */
#ifndef CAR_MCU_SERVICE_H_
#define CAR_MCU_SERVICE_H_
#include <stdlib.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <string>
#include <binder/IBinder.h>
#include "service/IMcuHardService.h"
#include "common/Common.h"
#include "utils/ZMutex.h"
#include "service/RemoteServiceList.h"
#include "common/ThreadPool.h"
#include "common/MsgData.h"
#include "common/MsgQueue.h"
#include "device/McuHDDevice.h"


class McuHardService: public BnMcuHardService, public ThreadPool, public IMcuUpdateDevListener
{
    public:
        virtual ~McuHardService();

        static McuHardService *getInstance();

        void init();
        //mcu消息回调
        virtual void onRecMcuVersion(const std::string soft, const std::string hard);
        virtual void onRecMcuUpdateState(int status);
    protected:
        //注册代理
        int32_t registRemoteProxy(sp<IBinder> proxy, std::string &pckName);
        //注销代理
        int32_t unRegistRemoteProxy(sp<IBinder> proxy, std::string &pckName);
        //请求MCU版本信息
        int32_t requestMcuVersion();
        //请求MCU版本更新
        int32_t requestMcuUpdate(std::string binPath);        
        
        //线程的运行实体
        virtual void run(TThread * thread);
    private:
        McuHardService();
    private:
        static McuHardService *s_ptrService;
        //远程服务列表
        static android::sp<RemoteServiceList>  s_vBinders;
        static ZMutex   s_ServiceLock; 
        //消息队列
        MsgQueue<MsgData> m_MsgQueue;
    private:
        McuHDDevice *m_ptrDev;
};
#endif
