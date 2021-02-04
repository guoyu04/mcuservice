/**
 * BaseUsbDevice.cpp
 *
 * USB设备节点类
 * Created by guoyu on 2019/10/27.
 */

#include "device/BaseUsbDevice.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"
#include "decoupler/DecouplerManager.h"

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.device.BaseUsbDevice"

#define DEV_MCU_STATUS_NURMAL       0x00                         //MCU状态：正常
#define DEV_MCU_STATUS_ERROR        0x01                         //MCU状态：异常
#define DEV_MCU_STATUS_NO_SUP       0x02                         //MCU状态：不支持
#define CHECK_MCU_COUNT 3
/**
 * 构造函数
 */
BaseUsbDevice::BaseUsbDevice()
{
    UsbMcuCenter::getInstance()->addUsbMcuDataListener(this);
}


BaseUsbDevice::~BaseUsbDevice()
{
}

/**
 * 获取can ID，这里会转换小端到大端序
 * @param [IN] recData 接收到的数据
 */
int BaseUsbDevice::getCanID(const char *recData)
{
    int canID = 0;
    //LOGD(LOG_TAG,"onRecDeviceData: ID[%02X%02X%02X%02X]",recData[5],recData[4],recData[3],recData[2]);
    bigChar2ToInt(recData+MSG_HEAD_LEN, canID);
    return canID;
}
/**
 * 向设备节点发送数据
 * @param [IN] pchSendData 发送数据缓存
 * @param [IN] iSendLen 发送数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int BaseUsbDevice::sendData(char *pchSendData, int iSendLen)
{
    if(NULL == pchSendData)
    {
        LOGE(LOG_TAG,"Pointer pchSendData is null. ");
        return RTN_FAIL;
    }

    int iRet =DecouplerManager::getInstance()->sendDataToSerial(pchSendData, iSendLen);
    if(RTN_SUCCESS != iRet)
    {
        LOGE(LOG_TAG,"Send data to device failed. ");
    }

    return iRet;
}

void BaseUsbDevice::onRecUsbMcuData(const char * data, int len)
{
}
