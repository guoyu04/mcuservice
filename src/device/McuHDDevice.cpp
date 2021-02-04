/**
 * McuHDDevice.cpp
 * MCU节点操作 
 * Created by guoyu on 2019/11/21.
 */
#include <stdio.h>
#include "device/McuHDDevice.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.device.McuHDDevice"

#define DEV_CMD_TYPE                0x03                  // 指令功能码
//节点子命令
#define DEV_SUBCMD_MCU_VER          0x81                        // 子命令：MCU版本查询
#define DEV_SUBCMD_MCU_REBOOT       0x82                        // 子命令：MCU重启
#define DEV_SUBCMD_UPDATE_REQ       0x83                        // 子命令：MCU升级程序

#define DEV_SUBCMD_UPDATE_DATA      0x85                        // 子命令：发送升级数据

#define DEV_SUBCMD_UPDATE_GET       0x04                         // 子命令: MCU索要升级数据
#define DEV_SUBCMD_UPDATE_FINISH    0x06                         // 子命令: MCU更新完成

#define MAX_UPDATE_PCK_LEN          128

/**
 * 构造函数
 */
McuHDDevice::McuHDDevice()
{
    m_pListener = NULL; 
}

/**
 * 析构函数
 */
McuHDDevice::~McuHDDevice()
{
}

/**
 * 设置mcu回调监听
 * @param listener 回调监听
 */
void McuHDDevice::setMcuUpdateDevListener(IMcuUpdateDevListener * listener)
{
    m_pListener = listener;
}

/**
 * 请求mcu版本，异步返回
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int McuHDDevice::requestMcuVersion()
{
    int iRet = RTN_FAIL;
  
    char cmdBuf[2] = {0};
    cmdBuf[0] = DEV_CMD_TYPE;//命令
    cmdBuf[1] = DEV_SUBCMD_MCU_VER;//子命令
    //DebugBuffer(LOG_TAG, "request mcu version: send data:", cmdBuf,sizeof(cmdBuf));
    iRet = sendData(cmdBuf, sizeof(cmdBuf));
    return iRet;
}

/**
 * 请求mcu更新，异步返回
 * @param binLen 升级文件长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int McuHDDevice::requestMcuUpdate(std::string binPath)
{
    int iRet = RTN_FAIL;
    
    //bin文件长度
    if(binPath.length() <= 0)
    {
        LOGE(LOG_TAG,"update mcu file path is empty");
        return iRet;
    }
    m_strBinPath = binPath;
    int binLen = Utils::getFileLen(binPath.c_str());
    LOGD(LOG_TAG,"update mcu file length[%d], path[%s]", binLen, binPath.c_str());
    if(binLen <= 0)
    {
        return iRet;
    }
    char cmdBuf[6] = {0};
    cmdBuf[0] = DEV_CMD_TYPE;//命令
    cmdBuf[1] = DEV_SUBCMD_UPDATE_REQ;//子命令
    cmdBuf[2] = binLen & 0x0FF; //命令值
    cmdBuf[3] = (binLen>>8)& 0x0FF;
    cmdBuf[4] = (binLen>>16)& 0x0FF;
    cmdBuf[5] = (binLen>>24)& 0x0FF;
   
    //DebugBuffer(LOG_TAG, "request mcu update: send data:", cmdBuf,sizeof(cmdBuf));
    iRet = sendData(cmdBuf, sizeof(cmdBuf));
    return iRet;
}

/**
 * 请求mcu重启
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int McuHDDevice::requestMcuReboot()
{
    int iRet = RTN_FAIL;
    char cmdBuf[2] = {0};
    cmdBuf[0] = DEV_CMD_TYPE;//命令
    cmdBuf[1] = DEV_SUBCMD_MCU_REBOOT;//子命令
    //DebugBuffer(LOG_TAG, "request mcu reboot: send data:", cmdBuf,sizeof(cmdBuf));
    iRet = sendData(cmdBuf, sizeof(cmdBuf));
    return iRet;
}

/**
 * 向mcu发送更新数据
 * 消息体为4位，数据置发一包可能发送不完，需要跟进偏移量发送多次，offset是mcu请求时发送的偏移值
 * @param offset 偏移地址
 * @param data 数据
 * @param dataLen 数据长度
 */
int McuHDDevice::sendUpdateData(int offset, char * data, int dataLen)
{
    int iRet = RTN_FAIL;
    char *cmdBuf = new char[6+dataLen];

    char * pos = cmdBuf;
    *pos++ = DEV_CMD_TYPE;//命令
    *pos++ = DEV_SUBCMD_UPDATE_DATA;//子命令
    *pos++ = offset & 0x0FF; //命令值
    *pos++ = (offset>>8)& 0x0FF;
    *pos++ = (offset>>16)& 0x0FF;
    *pos++ = (offset>>24)& 0x0FF;
    memcpy(pos, data, dataLen);
    DebugBuffer(LOG_TAG, "send mcu update data: send data:", cmdBuf,5+dataLen);
    iRet = sendData(cmdBuf, 6+dataLen);
    delete []cmdBuf;
    return iRet;
}

/**
 * 获取升级文件内容
 * @param [in] offset 文件偏移
 * @parma [out] data 数据内容
 * @param [out] dataLen 数据长度
 */
int McuHDDevice::getFileData(int offset, char * data, int &dataLen)
{
    int iRet = RTN_FAIL;
    if(m_strBinPath.length() <= 0)
    {
        LOGE(LOG_TAG,"getFileData: update mcu file path is empty");
        return iRet;
    }

    FILE *fp = fopen(m_strBinPath.c_str(), "rb");
	if (NULL == fp)
	{
        return iRet;
    }

    fseek(fp, offset, SEEK_CUR); 
    dataLen = fread(data, 1, MAX_UPDATE_PCK_LEN, fp);
    if(dataLen > 0)
    {
        iRet = RTN_SUCCESS;
    }
    fclose(fp);
    return iRet;
}

/**
 * 处理MCU版本信息
 */
void McuHDDevice::processMcuVersion(const char * recData)
{
    //int softVer = (recData[0] & 0xff) + (recData[1] & 0xff) * 256;
    char soft[10] = {0};
    char hard[10] = {0};
    sprintf(soft, "%d.%d", recData[1], recData[0]);
    sprintf(hard, "%d.%d", recData[3], recData[2]);
    LOGD(LOG_TAG,"processMcuVersion: soft[%s], hard[%s]", soft, hard);
    if(m_pListener != NULL)
    {
        m_pListener->onRecMcuVersion(soft, hard);
    }
}

/**
 * 处理MCU更新请求返回结果
 */
void McuHDDevice::processUpdateReqResult(const char * recData)
{
    LOGD(LOG_TAG,"processUpdateReqResult: result[%02x]",recData[0]);
    if(recData[0] == 0x01 && m_pListener != NULL)
    {
        m_pListener->onRecMcuUpdateState(RTN_FAIL);
    }
}

/**
 * 处理MCU向SOC索要升级数据
 */
void McuHDDevice::processUpdateGetData(const char * recData)
{
    int offset = (recData[0] & 0xff) + (recData[1] & 0xff) * 256 
                    + (recData[2] & 0xff) * 256 * 256
                    + (recData[3] & 0xff) * 256 * 256 * 256;
    LOGD(LOG_TAG,"processUpdateGetData: offset[%d]",offset);
    char sendData[MAX_UPDATE_PCK_LEN] = {0};
    int sendLen = 0;
    int iRet = getFileData(offset, sendData, sendLen);
    if(iRet == RTN_SUCCESS)
    {
        sendUpdateData(offset, sendData, sendLen);
    }
    else
    {
        if(m_pListener != NULL)
        {
            m_pListener->onRecMcuUpdateState(RTN_FAIL);
        }
    }
}

/**
 * 处理MCU更新完成
 */
void McuHDDevice::processUpdateFinish(const char * recData)
{
    LOGD(LOG_TAG,"processUpdateFinish: result[%02x]",recData[0]);
    requestMcuReboot();
    if(m_pListener != NULL)
    {
        m_pListener->onRecMcuUpdateState(RTN_SUCCESS);
    }
}

/**
 * 线程执行体
 */
void McuHDDevice::onRecUsbMcuData(const char * recData, int len)
{
    if(recData == NULL || recData[0] != DEV_CMD_TYPE)
    {
        return;
    }
    DebugBuffer(LOG_TAG, "----zzh",  (char*)recData, len);
    recData++;
    switch(recData[0])
    {
        case DEV_SUBCMD_MCU_VER&0x0F:
            processMcuVersion(++recData);
            break;
        case DEV_SUBCMD_UPDATE_REQ&0x0F:
            processUpdateReqResult(++recData);
            break;
        case DEV_SUBCMD_UPDATE_GET:
            processUpdateGetData(++recData);
            break;
        case DEV_SUBCMD_UPDATE_DATA&0x0F:
            break;
        case DEV_SUBCMD_UPDATE_FINISH:
            processUpdateFinish(++recData);
            break;
    }
}
