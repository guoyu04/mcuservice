/**
 * UsbSerial.cpp
 *
 * 串口类，负责串口通信，维护一个Decoupler，代表当前串口的分包器
 * Created by guoyu on 2019/8/27.
 */

#include "serial/UsbSerial.h"
#include "decoupler/Decoupler.h"
#include "utils/ZLog.h"

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.serial.UsbSerial"

UsbSerial::UsbSerial()
{
}

UsbSerial::UsbSerial(const char *pchSrlPrtNm, int iBaud, Decoupler *decoupler)
{
    if(NULL == pchSrlPrtNm)
    {
        LOGE(LOG_TAG,"UsbSerial constructor has null pointer. ");
        return;
    }

    strcpy(m_achSerialName, pchSrlPrtNm);

    m_iConnectState = DEV_OPEN_FAIL;  // 初始化串口未连接
    m_iFD    = FD_INVALID;          // 初始化串口连接fd无效
    m_iBaud = iBaud;
    m_ptrDecoupler = decoupler;
}

UsbSerial::~UsbSerial()
{
    if(NULL != m_ptrDecoupler)
    {
        delete m_ptrDecoupler;
    }
}

/**
 * 设置串口路径
 */
void UsbSerial::setSerialName(const char * srlPrtNm)
{
    if(NULL == srlPrtNm)
    {
        return;
    }
    strcpy(m_achSerialName, srlPrtNm);
}

/**
 * 打开串口
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::openSerial()
{
    int iRet = RTN_FAIL;
    m_iFD = open(m_achSerialName, O_RDWR | O_NOCTTY | O_NDELAY);
    if(m_iFD < 0)
    {
        LOGE(LOG_TAG,"Open stm32 serial port %s error. ", m_achSerialName);
        return RTN_FAIL;
    }
    if(fcntl(m_iFD, F_SETFL, 0) < 0) //恢复为阻塞模式
    {
        LOGE(LOG_TAG,"(fcntl(m_iFD, F_SETFL, 0) failed. ");
        closeSerial();

        return RTN_FAIL;
    }

    iRet = configSerial(m_iBaud);
    if(RTN_SUCCESS != iRet)
    {
        LOGE(LOG_TAG,"Cfg serial port failed. ");
        return RTN_FAIL;
    }

    m_iConnectState = DEV_OPEN_SUCCESS;
    LOGD(LOG_TAG,"Connect to serial port %s success. ", m_achSerialName);
    return RTN_SUCCESS;
}

/**
 * 配置串口
 * @param iBaud 串口速率
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::configSerial(int iBaud)
{
    int aiNameArray[]  = {115200, 57600, 38400, 19200, 9600, 4800, 2400};
    int aiSpeedArray[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400};
    struct termios tOptions;
    int iCfgSpeed = 0x00;
    bzero(&tOptions, sizeof(tOptions));

    for(unsigned int uiLoop = 0; (uiLoop < sizeof(aiSpeedArray) / sizeof(int)); uiLoop++)
    {
        if(iBaud == aiNameArray[uiLoop])
        {
            iCfgSpeed = aiSpeedArray[uiLoop];
        }
    }

    // 对原来配置处理
    tcgetattr(m_iFD, &tOptions);
    tcflush(m_iFD, TCIOFLUSH);

    // 配置波特率
    cfsetispeed(&tOptions, iCfgSpeed);
    cfsetospeed(&tOptions, iCfgSpeed);
    // 立即生效
    tcsetattr(m_iFD, TCSANOW, &tOptions);
    tcflush(m_iFD, TCIOFLUSH);

    // 对原来配置处理
    tcgetattr(m_iFD, &tOptions);
    tcflush(m_iFD, TCIOFLUSH);

    // 8个数据位
    tOptions.c_cflag &= ~CSIZE;
    tOptions.c_cflag |= CS8;//8个数据位

    // 1个停止位
    tOptions.c_cflag &= ~CSTOPB;
    // 无校验
    tOptions.c_cflag &= ~PARENB; // 关闭校验
    tOptions.c_iflag &= ~INPCK;  // 关闭奇偶校验
    // 设置最小字符和等待时间
    tOptions.c_cc[VTIME] = 0;// 15s等待
    tOptions.c_cc[VMIN] = 0; // 最小字符为1

    cfmakeraw(&tOptions); // 设置为原始模式
    // c_cflag
    tOptions.c_cflag &= ~CRTSCTS; // 禁止硬件流控
    tOptions.c_cflag |= (CLOCAL | CREAD); // 激活选项

    tcflush(m_iFD, TCIFLUSH);
    if((tcsetattr(m_iFD, TCSANOW, &tOptions)) != 0)
    {
        LOGE(LOG_TAG,"tcsetattr m_iFD!");
        closeSerial();

        return RTN_FAIL;
    }

    return RTN_SUCCESS;
}

/**
 * 断开串口
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::closeSerial()
{
    if((FD_INVALID != m_iFD) && (DEV_OPEN_SUCCESS == m_iConnectState))
    {
        close(m_iFD);

        m_iFD    = FD_INVALID;
        m_iConnectState = DEV_OPEN_FAIL;
    }

    return RTN_SUCCESS;
}

/**
 * 向串口发送数据
 * @param [IN] pchSendData 发送数据缓存
 * @param [IN] iSendLen 发送数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::sendData(char *pchSendData, int iSendLen)
{
    if(DEV_OPEN_SUCCESS != m_iConnectState)
    {
        LOGE(LOG_TAG,"Serial port connect status is not ok, can't send data. ");
        return RTN_FAIL;
    }

    if(NULL == pchSendData)
    {
        LOGE(LOG_TAG,"Pointer pchSendData is null. ");
        return RTN_FAIL;
    }
    DebugBuffer(LOG_TAG, "sendData", pchSendData,iSendLen);
    int iRet = writeN(m_iFD, pchSendData, iSendLen);
    if(RTN_SUCCESS != iRet)
    {
        LOGE(LOG_TAG,"Send data to serial port failed. ");
        closeSerial();
    }

    return RTN_SUCCESS;
}

/**
 * 从串口接收数据
 * @param [IN] iRecvBuffLen 接收缓存长度
 * @param [IN] pchRecvBuff 接收缓存
 * @param [OUT] iRecvLen 接收数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::recvData(char *pchRecvBuff, int iRecvBuffLen, int &iRecvLen)
{
    if(DEV_OPEN_SUCCESS != m_iConnectState)
    {
        LOGE(LOG_TAG,"Serial port connect status is not ok, can't recv data. ");
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
        LOGE(LOG_TAG,"Read serila port error");
        closeSerial();

        return RTN_FAIL;
    }

    return RTN_SUCCESS;
}

/**
 * 向串口发送数据
 * @param [OUT] iCnctStus 接收数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::getConnectStatus()
{
    return m_iConnectState;
}

/**
 * 获取串口连接的描述符
 * @param [OUT] iFd 串口连接描述符
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::getConnectFd(int &iFd)
{
    if(DEV_OPEN_SUCCESS == m_iConnectState)
    {
        iFd = m_iFD;
    }
    else
    {
        LOGE(LOG_TAG,"Serial port connect status is not ok, get fd faild. ");
        iFd = FD_INVALID;

        return RTN_FAIL;
    }

    return RTN_SUCCESS;
}

/**
 * 注册串口命令字处理函数
 * @param [IN] chCmd 串口命令字
 * @param [IN] pfnSrlPrtFunc 命令字对应处理函数
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
/*int UsbSerial::RegSerialPortCmdFunc(char chCmd, PSrlPrtFunc pfnSrlPrtFunc)
{
    int iRet = m_cSrlPtProc.RegCmdProcFunc(chCmd, pfnSrlPrtFunc);
    if(RTN_SUCCESS != iRet)
    {
        LOGE(LOG_TAG,"Serial cmd %2x function register failed. ", chCmd);
        return RTN_FAIL;
    }

    return RTN_SUCCESS;
}*/

/**
 * 处理串口数据
 * @param [IN] chCmd 串口命令字
 * @param [IN] pchSendData 命令字对应数据
 * @param [IN] iDataLen 数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
/*int UsbSerial::ProUsbSerialPortData(char chCmd, char *pchSendData, int iDataLen)
{
    int iRet = m_cSrlPtProc.ProcCmdData(chCmd, pchSendData, iDataLen);
    if(RTN_SUCCESS != iRet)
    {
        LOGE(LOG_TAG,"Serial cmd %2x proc failed. ", chCmd);
        PrintBuffer((char*)("Cmd data"), pchSendData, iDataLen);

        return RTN_FAIL;
    }

    return RTN_SUCCESS;
}*/

/**
 * 将数据放入分包器
 * @param [IN] pchData 放入分包器的数据缓存
 * @param [IN] iDataLen 放入数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::putDataToDcplr(char *pchData, int iDataLen)
{
    int iRet = m_ptrDecoupler->putData(pchData, iDataLen);
    if(RTN_SUCCESS != iRet)
    {
        LOGE(LOG_TAG,"Put data into decoupler failed. ");
        return RTN_FAIL;
    }

    return RTN_SUCCESS;
}

/**
 * 获取分包数据
 * @param [OUT] pchPackData 包数据缓存
 * @param [IN] iPackBufSize 包数据缓存长度
 * @param [OUT] iPackLen 数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::getPackInfo(char *pchPackData, int iPackBufSize, int &iPackLen)
{
    return m_ptrDecoupler->getPackInfo(pchPackData, iPackBufSize, iPackLen);
}

/**
 * 分包器是否为空
 * @return true 分包器已空
 *         false 分包器未空
 */
bool UsbSerial::isDecouplerEmpty()
{
    return m_ptrDecoupler->isDecouplerEmpty();
}

/**
 * 获取包内容的实际长度:除去分隔符、包长度标识后的长度
 * @return 包内容的长度
 */
short UsbSerial::getPacketContentLen()
{
    return m_ptrDecoupler->getPacketContentLen();
}

/**
 * 获取包消息体的实际长度:除去分隔符、包长度标识、校验码后的长度
 */
short UsbSerial::getPackageBodyLen()
{
    return m_ptrDecoupler->getPackageBodyLen();
}

/**
 * 注册串口连接成功处理函数
 * @param [IN] pfnCntScsFunc 放入分包器的数据缓存
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
/*int UsbSerial::RegCnctSucsFunc(PSrlCntScsFunc pfnCntScsFunc)
{
    if(NULL == pfnCntScsFunc)
    {
        LOGE(LOG_TAG,"RegCnctSucsFunc input param has null pointer. ");
        return RTN_FAIL;
    }

    m_pfnSrlCntScsFunc = pfnCntScsFunc;

    return RTN_SUCCESS;
}*/

/**
 * 写n个byte到描述符
 * @param [IN] iFd 描述符
 * @param [IN] pchBuff 保存写入内容的缓存
 * @param [IN] iWriteLen 写入长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbSerial::writeN(int iFd, char *pchBuff, int iWriteLen)
{
    int  iLeft;
    int  iWriteN;
    char *pchCur;

    iLeft  = iWriteLen;
    pchCur = pchBuff;

    while(iLeft > 0)
    {
        iWriteN = write(iFd, pchCur, iLeft);
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
int UsbSerial::readN(int iFd, char *pchBuff, int iBuffLen, int &iReadLen)
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
