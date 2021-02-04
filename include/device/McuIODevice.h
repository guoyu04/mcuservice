/**
 * McuIODevice.h
 * MCU IO节点操作 
 * Created by guoyu on 2019/11/21.
 */

#ifndef __MCU_IO_DEVICE_H__
#define __MCU_IO_DEVICE_H__
#include "device/BaseUsbDevice.h"

class IMcuIODevListener : public virtual android::RefBase
{
public:  
    virtual void onRecKeyEvent(char key, char state) = 0;
};

class McuIODevice : public BaseUsbDevice {
	public:
		McuIODevice();
		virtual ~McuIODevice();
        //设置mcu回调监听
        void setMcuIODevListener(IMcuIODevListener * listener);
        int requestKeyState();//请求key状态
       
    protected:
         virtual void onRecUsbMcuData(const char * recData, int len);
    private:
         //处理key按键
         void processKeyEvent(const char * recData);
    private:
         android::sp<IMcuIODevListener> m_pListener;
         std::string m_strBinPath;
};


#endif
