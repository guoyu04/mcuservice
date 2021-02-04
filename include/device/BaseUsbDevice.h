/**
 * BaseUsbDevice.h
 *
 * USB设备节点类
 * Created by guoyu on 2019/10/27.
 */

#ifndef __BASE_USB_DEVICE_H__
#define __BASE_USB_DEVICE_H__
#include <utils/RefBase.h>
#include "common/Common.h"
#include "common/TThread.h"
#include "mcu/UsbMcuCenter.h"

/*----------------------------------类定义-----------------------------------*/

class BaseUsbDevice : public IUsbMcuDataListener
{
public:
    BaseUsbDevice();
    virtual ~BaseUsbDevice();
    
    int sendData(char *pchSendData, int iSendLen);   // 从设备节点发送数据
    /**
     * 获取can ID，这里会转换小端到大端序
     * @param [IN] recData 接收到的数据
     */
    int getCanID(const char * recData);
protected:
     virtual void onRecUsbMcuData(const char * data, int len);
};


#endif /* __SERIAL_H__ */




