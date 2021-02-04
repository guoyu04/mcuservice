/**
 * McuIODevice.cpp
 * MCU节点操作 
 * Created by guoyu on 2019/11/21.
 */
#include <stdio.h>
#include "device/McuIODevice.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.device.McuIODevice"

#define DEV_CMD_TYPE                 0x11                        // 指令功能码
#define DEV_SUBCMD_IO_KEY1           0x01                       // 子命令: Key1
#define DEV_SUBCMD_IO_KEY2           0x02                       // 子命令: Key2
#define DEV_SUBCMD_IO_KEY3           0x03                       // 子命令: Key3

/**
 * 构造函数
 */
McuIODevice::McuIODevice()
{
    m_pListener = NULL; 
}

/**
 * 析构函数
 */
McuIODevice::~McuIODevice()
{
}

/**
 * 设置mcu回调监听
 * @param listener 回调监听
 */
void McuIODevice::setMcuIODevListener(IMcuIODevListener * listener)
{
    m_pListener = listener;
}

/**
 * 请求key状态
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int McuIODevice::requestKeyState()
{
    int iRet = RTN_FAIL;
  
    char cmdBuf[2] = {0};
    cmdBuf[0] = DEV_CMD_TYPE;//命令
    cmdBuf[1] = DEV_SUBCMD_IO_KEY1;//子命令
    //DebugBuffer(LOG_TAG, "request mcu version: send data:", cmdBuf,sizeof(cmdBuf));
    iRet = sendData(cmdBuf, sizeof(cmdBuf));
    return iRet;
}

/**
 * 处理key按键
 */
void McuIODevice::processKeyEvent(const char * recData)
{
    char event = recData[0];
    char state = recData[1];
    LOGD(LOG_TAG,"processIOState: key[%0X] state[%0X]", event, state);
    if(m_pListener != NULL)
    {
        m_pListener->onRecKeyEvent(event&0x0F, state);
    }
}

/**
 * 线程执行体
 */
void McuIODevice::onRecUsbMcuData(const char * recData, int len)
{
    if(recData == NULL || recData[0] != DEV_CMD_TYPE)
    {
        return;
    }
    DebugBuffer(LOG_TAG, "----zzh",  (char*)recData, len);
    recData++;
    switch(recData[0])
    {
        case DEV_SUBCMD_IO_KEY1&0x0F:
        case DEV_SUBCMD_IO_KEY2&0x0F:
        case DEV_SUBCMD_IO_KEY3&0x0F:
            processKeyEvent(recData);
            break;
    }
}
