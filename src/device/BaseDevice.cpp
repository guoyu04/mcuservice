/**
 * BaseDevice.cpp
 *
 * 设备节点类
 * Created by guoyu on 2019/10/27.
 */

#include "device/BaseDevice.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.device.BaseDevice"

#define DEV_MCU_STATUS_NURMAL       0x00                         //MCU状态：正常
#define DEV_MCU_STATUS_ERROR        0x01                         //MCU状态：异常
#define DEV_MCU_STATUS_NO_SUP       0x02                         //MCU状态：不支持
#define CHECK_MCU_COUNT 3
/**
 * 构造函数
 *
 * 本项目暂不用
 */
BaseDevice::BaseDevice(): m_iOpenState(DEV_OPEN_FAIL), m_iFD(FD_INVALID)
{
    m_ptrTThread = new TThread();
}

/**
 * 析构函数
 */
BaseDevice::BaseDevice(std::string devName):m_strDevName(devName), m_iOpenState(DEV_OPEN_FAIL), m_iFD(FD_INVALID)
{
    if (m_strDevName.empty())
    {
        LOGE(LOG_TAG,"device name is empty. ");
        return;
    }
    m_ptrTThread = new TThread();
}

BaseDevice::~BaseDevice()
{
    if(NULL != m_ptrTThread)
   {
       delete m_ptrTThread;
   }
}

/**
 * 设置设备节点路径
 * @param [IN] devName 节点路径 
 */
void BaseDevice::setDeviceName(std::string devName)
{
    if (m_strDevName.empty())
    {
        LOGE(LOG_TAG,"device name is empty. ");
        return;
    }
    m_strDevName = devName;
}

/**
 * 打开设备节点
 * @param isCheckMcu 是否检测节点状态
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int BaseDevice::openDevice(bool isCheckMcu)
{
    do
    {
        m_iOpenState = DEV_OPEN_FAIL;
        m_iFD = open(m_strDevName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if(m_iFD < 0)
        {
            LOGE(LOG_TAG,"Open %s device error. ",m_strDevName.c_str());
            break;
        }
        /*else
        {
            if(fcntl(m_iFD, F_SETFL, 0) < 0) //恢复为阻塞模式
            {
                LOGE(LOG_TAG,"(fcntl(m_iFD, F_SETFL, 0) failed. ");
                closeDevice();
                m_iOpenState = DEV_OPEN_FAIL;
                break;
            }
        }*/
        //打开节点成功
        if(isCheckMcu && checkMcuStatus() != RTN_SUCCESS)
        {
           LOGD(LOG_TAG,"Open to device %s fail status[%d]. ",m_strDevName.c_str(), m_iOpenState);
           break;
        }
        if(!m_ptrTThread->start(this)) break; //启动线程
        m_iOpenState = DEV_OPEN_SUCCESS;
        LOGD(LOG_TAG,"Open to device %s success. ",m_strDevName.c_str());
    }while(0);
    return m_iOpenState;
}

/**
 * 断开设备节点
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int BaseDevice::closeDevice()
{
    if((FD_INVALID != m_iFD) && (DEV_OPEN_SUCCESS == m_iOpenState))
    {
        close(m_iFD);

        m_iFD    = FD_INVALID;
        m_iOpenState = DEV_OPEN_FAIL;
    }

    return RTN_SUCCESS;
}

/**
 * 判断设备是否打开 
 * @return 打开返回true，未打开返回false
 */
bool BaseDevice::isDeviceOpen()
{
    return m_iOpenState == DEV_OPEN_SUCCESS;
}

/**
 * 向远程设备查询是否是否可用
 */
int BaseDevice::checkMcuStatus()
{
    int iRet = RTN_FAIL;
    int iFdMax;
    FdSet fdReadSet;
    TTimeVal tTimeOut;
    int checkCount = CHECK_MCU_COUNT;
    char cmdBuf[1] = {0};
    char recBuf[2] = {0};
    int  iRecvNum = 0;
    //发送查询状态指令 
    cmdBuf[0] = DEV_CMD_STATUS_QUERY;//子命令
    LOGD(LOG_TAG,"checkMcuStatus: device %s", m_strDevName.c_str());
    iRet = writeN(m_iFD, cmdBuf, sizeof(cmdBuf));
    if(RTN_SUCCESS != iRet) return iRet;
    iFdMax = m_iFD;
    //读取
    while(checkCount > 0)
    {
        checkCount--;
        FD_ZERO(&fdReadSet);//将set清零
        FD_SET(m_iFD, &fdReadSet);//m_iFD加入set
        tTimeOut.tv_sec  = 0;
        tTimeOut.tv_usec = 100*1000;
        iRecvNum = select((iFdMax + 1), &fdReadSet, NULL, NULL, &tTimeOut);
        if(0 == iRecvNum)//超时
        {
            continue;
        }
        else if(iRecvNum < 0)
        {
            LOGE(LOG_TAG,"select error happened errno = %d. ", errno);
            continue;
        }
        else
        {
            iRecvNum = read(m_iFD, recBuf, sizeof(recBuf));
            if((iRecvNum < 0) && (errno != EINTR))
            {
                LOGE(LOG_TAG,"Read device data error");
                Utils::thrsleep(50);
                continue;
            }
            DebugBuffer(LOG_TAG, "checkMcuStatus: rec data", recBuf,sizeof(recBuf));
            if(recBuf[0] == DEV_CMD_STATUS_REC)
            {
                if(recBuf[1] == DEV_MCU_STATUS_ERROR)
                {
                    m_iOpenState = DEV_OPEN_MCU_ERROR;
                    break;
                }
                else if(recBuf[1] == DEV_MCU_STATUS_NO_SUP)
                {
                    m_iOpenState = DEV_OPEN_MCU_NO_SUP;
                    break;
                }
                m_iOpenState == DEV_OPEN_SUCCESS;
            }
            break;
        }
    }
    return m_iOpenState;
}

/**
 * 尝试打开设备节点
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int BaseDevice::tryOpenDevice()
{
    int iRet = DEV_OPEN_SUCCESS;
    if(m_iOpenState == DEV_OPEN_MCU_ERROR)//如果因为mcu异常，则尝试打开设备
    {
        iRet = openDevice();
    }
    return iRet;
}

/**
 * 向设备节点发送数据
 * @param [IN] pchSendData 发送数据缓存
 * @param [IN] iSendLen 发送数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int BaseDevice::sendData(char *pchSendData, int iSendLen)
{
    if(DEV_OPEN_SUCCESS != m_iOpenState)
    {
        LOGE(LOG_TAG,"Device connect status is not ok, can't send data. ");
        return RTN_FAIL;
    }

    if(NULL == pchSendData)
    {
        LOGE(LOG_TAG,"Pointer pchSendData is null. ");
        return RTN_FAIL;
    }

    int iRet = writeN(m_iFD, pchSendData, iSendLen);
    if(RTN_SUCCESS != iRet)
    {
        LOGE(LOG_TAG,"Send data to device failed. ");
    }

    return RTN_SUCCESS;
}

/**
 * 从设备节点接收数据
 * @param [IN] iRecvBuffLen 接收缓存长度
 * @param [IN] pchRecvBuff 接收缓存
 * @param [OUT] iRecvLen 接收数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int BaseDevice::recvData(char *pchRecvBuff, int iRecvBuffLen, int &iRecvLen)
{
    if(DEV_OPEN_SUCCESS != m_iOpenState)
    {
        LOGE(LOG_TAG,"Device connect status is not ok, can't recv data. ");
        return RTN_FAIL;
    }

    if(NULL == pchRecvBuff)
    {
        LOGE(LOG_TAG,"Pointer pchRecvBuff is null. ");
        return RTN_FAIL;
    }

    iRecvLen = read(m_iFD, pchRecvBuff, iRecvBuffLen);
    if((iRecvLen < 0) && (errno != EINTR))
    {
        LOGE(LOG_TAG,"Read device data error");

        return RTN_FAIL;
    }

    return RTN_SUCCESS;
}

/**
 * 获取设备节点打开状态
 * @return 打开状态 
 */
int BaseDevice::getOpenStatus()
{
    return m_iOpenState;
}

/**
 * 获取设备节点打开的描述符
 * @param [OUT] iFd 设备节点打开描述符
 * @return 设备节点打开描述符
 */
int BaseDevice::getOpenFd()
{
    return m_iFD;
}


/**
 * 写n个byte到描述符
 * @param [IN] iFd 描述符
 * @param [IN] pchBuff 保存写入内容的缓存
 * @param [IN] iWriteLen 写入长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int BaseDevice::writeN(int iFd, char *pchBuff, int iWriteLen)
{
    int  iLeft;
    int  iWriteN;
    char *pchCur;

    iLeft  = iWriteLen;
    pchCur = pchBuff;

    while(iLeft > 0)
    {
        iWriteN = write(iFd, pchCur, iLeft);
        LOGD(LOG_TAG,"writeN length[%d]",iWriteN);
        if(iWriteN <= 0)
        {
            if((iWriteN < 0) && (errno == EINTR))
            {
                iWriteN = 0;
            }
            else
            {
                return RTN_FAIL;
            }
        }

        iLeft  -= iWriteN;
        pchCur += iWriteN;
    }

    return RTN_SUCCESS;
}

/**
 * 从描述符读n个byte
 * @param [IN] iFd 描述符
 * @param [IN] pchBuff 保存读取内容的缓存
 * @param [IN] iBuffLen 读取长度
 * @param [OUT] iReadLen 实际读取长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int BaseDevice::readN(int iFd, char *pchBuff, int iBuffLen, int &iReadLen)
{
    int  iLeft;
    int  iRead;
    char *pchCur;

    iLeft  = iBuffLen;
    pchCur = pchBuff;

    while(iLeft > 0)
    {
        iRead = read(iFd, pchCur, iLeft);
        if(iRead < 0)
        {
            if(errno == EINTR)
            {
                iRead = 0;
            }
            else
            {
                LOGE(LOG_TAG,"Read error");
                return RTN_FAIL;
            }
        }
        else if(iRead == 0)
        {
            break;
        }

        iLeft  -= iRead;
        pchCur += iRead;
    }

    iReadLen = iBuffLen - iLeft;

    return RTN_SUCCESS;
}

void BaseDevice::onRecDeviceData(char * redData, int recLen)
{
   //LOGD(LOG_TAG,"onRecDeviceData: [%s] %d", redData, recLen); 
}

/**
 * 线程执行体
 */
void BaseDevice::run(TThread * thread)
{
    int iFdMax;
    FdSet fdReadSet;
    int  iRecvNum = 0;
    iFdMax = m_iFD;
    //读取
    while(1)
    {
        LOGD(LOG_TAG,"run read data [%s]", m_strDevName.c_str());
        FD_ZERO(&fdReadSet);//将set清零
        FD_SET(m_iFD, &fdReadSet);//m_iFD加入set
        iRecvNum = select((iFdMax + 1), &fdReadSet, NULL, NULL, NULL);
        if(0 == iRecvNum)//超时
        {
            continue;
        }
        else if(iRecvNum < 0)
        {
            LOGE(LOG_TAG,"select error happened errno = %d. ", errno);
            continue;
        }
        else
        {
            char recBuf[PACK_LEN_MAX] = {0};
            memset(recBuf, 0, sizeof(recBuf));
            iRecvNum = read(m_iFD, recBuf, sizeof(recBuf));
            if((iRecvNum <= 0) && (errno != EINTR))
            {
                LOGE(LOG_TAG,"Read device data error");
                Utils::thrsleep(50);
                continue;
            }
            LOGD(LOG_TAG,"[%s] read data len %d", m_strDevName.c_str(), iRecvNum);
            onRecDeviceData(recBuf, iRecvNum); 
        }
    }
}
