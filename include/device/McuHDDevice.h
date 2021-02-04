/**
 * McuHDDevice.h
 * MCU节点操作 
 * Created by guoyu on 2019/11/21.
 */

#ifndef __MCU_DEVICE_H__
#define __MCU_DEVICE_H__
#include "device/BaseUsbDevice.h"

class IMcuUpdateDevListener : public virtual android::RefBase
{
public:  
    virtual void onRecMcuVersion(const std::string soft, const std::string hard) = 0;
    virtual void onRecMcuUpdateState(int status) = 0;
};

class McuHDDevice : public BaseUsbDevice {
	public:
		McuHDDevice();
		virtual ~McuHDDevice();
        //设置mcu回调监听
        void setMcuUpdateDevListener(IMcuUpdateDevListener * listener);
        int requestMcuVersion();//请求mcu版本，异步返回
        int requestMcuReboot();//请求mcu重启
        int requestMcuUpdate(std::string binPath);//请求mcu更新，异步返回
        int sendUpdateData(int offset, char * data, int dataLen);//向mcu发送更新数据
    protected:
         virtual void onRecUsbMcuData(const char * recData, int len);
    private:
         //处理MCU版本信息
         void processMcuVersion(const char * recData);
         //处理MCU更新请求返回结果
         void processUpdateReqResult(const char * recData);
         //处理MCU向SOC索要升级数据
         void processUpdateGetData(const char * recData);
         //处理MCU更新完成
         void processUpdateFinish(const char * recData);
         int getFileData(int offset, char * data, int &dataLen);
    private:
         android::sp<IMcuUpdateDevListener> m_pListener;
         std::string m_strBinPath;
};


#endif
