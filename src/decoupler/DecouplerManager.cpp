/**
 * DecouplerManager.cpp
 * MCU协议解析:     
 * 通过UsbSerial和串口通信，并创建一个Decoupler负责解包
 * 维护一个线程，监听串口是否可读，如果可读则读取数据让Decoupler解包后放入UsbMcuCenter中
 * Created by guoyu on 2019/8/27.
 */
#include "decoupler/DecouplerManager.h"
#include "decoupler/UsbDecoupler.h"
#include "serial/UsbSerial.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"
#include "mcu/UsbMcuCenter.h"

#include <string>
using std::string;

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.decoupler.DecouplerManager"

#define PACKAGE_PREFIX 0xFA
#define PACKAGE_PREFIX_LEN 2
//前缀长度+包长度的字节数(2+2)
#define PACKAGE_HEAD_LEN PACKAGE_PREFIX_LEN+PACK_LEN_LEN
#define MCU_SERIAL_PATH0 "/dev/ttyMT2"
//#define MCU_SERIAL_PATH1 "/dev/ttyACM1"

DecouplerManager* DecouplerManager::m_ptrMcuDpl = NULL;
CLock DecouplerManager::m_Lock;


DecouplerManager::DecouplerManager()
{
    char prefix[PACKAGE_PREFIX_LEN] = {PACKAGE_PREFIX, PACKAGE_PREFIX};
    Decoupler* decoupler = makeDecoupler(prefix, sizeof(prefix));
    m_ptrSerial = new UsbSerial(MCU_SERIAL_PATH0, 115200, decoupler);
    m_ptrTThread = new TThread();
}

DecouplerManager::~DecouplerManager()
{
   if(NULL != m_ptrSerial)
   {
       delete m_ptrSerial;
   }
   if(NULL != m_ptrTThread)
   {
       delete m_ptrTThread;
   }
}

DecouplerManager* DecouplerManager::getInstance()
{
    if( NULL == m_ptrMcuDpl )
	{
		m_Lock.Lock();
		
		if( NULL == m_ptrMcuDpl ) 
		{
			 m_ptrMcuDpl = new DecouplerManager();
		}

		m_Lock.Unlock();
	}

	return m_ptrMcuDpl;
}

/**
 * 初始化分包器
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int DecouplerManager::init()
{
    int iResult = RTN_FAIL;
    do
    {
        iResult = m_ptrSerial->openSerial();
        m_ptrTThread->start(this);
    }while(0);
    return iResult;
}

/**
 * 向串口发送数据
 * @param [IN] pchSendData 发送数据缓存
 * @param [IN] iSendLen 发送数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int DecouplerManager::sendDataToSerial(char *pchSendData, int iSendLen)
{
    int iRet = RTN_FAIL;
    if(m_ptrSerial->getConnectStatus() != DEV_OPEN_SUCCESS)
    {
        iRet = m_ptrSerial->openSerial();
        if(iRet != RTN_SUCCESS || m_ptrSerial->getConnectStatus() != DEV_OPEN_SUCCESS)
        {
            return iRet;
        }
    }
    int pckLen = PACKAGE_HEAD_LEN+iSendLen+PACK_CHECKCODE_LEN;
    char *data = new char[pckLen];
    memset(data, 0x00, pckLen);
    memset(data, PACKAGE_PREFIX, PACKAGE_PREFIX_LEN);//放入前缀
    shortToBigChar2(pckLen, data+PACKAGE_PREFIX_LEN);//放入长度
    memcpy(data+PACKAGE_PREFIX_LEN+PACK_LEN_LEN, pchSendData, iSendLen);
    short checkCode = Utils::calAndCheckCode(data, pckLen-PACK_CHECKCODE_LEN);
    shortToBigChar2(checkCode, data+pckLen-2);
    iRet = m_ptrSerial->sendData(data, pckLen);
    delete []data;
    return iRet;
}

/**
 * make一个MCU分包器
 * @return Decoupler
 */
Decoupler * DecouplerManager::makeDecoupler(char * prefix, int prefixLen)
{
    return new UsbDecoupler(prefix, prefixLen, (PACK_LEN_MAX * 2));
}

/**
 * run回调
 */
void DecouplerManager::run(TThread * thread)
{
    int         iSerFD;
    int         iFdMax;
    int         iResult;
    FdSet       fdReadSet;
    TTimeVal    tTimeOut;
    int         iOpenCount = 5;
    LOGD(LOG_TAG,"run");
    while(1)
    {
        iResult = m_ptrSerial->getConnectFd(iSerFD);
        if(iResult != RTN_SUCCESS)
        {
            if(iOpenCount <= 0) return;
            iOpenCount --;
            Utils::thrsleep(1000*10);
            iResult = m_ptrSerial->openSerial();
            continue;
        }
        FD_ZERO(&fdReadSet);
        iFdMax = iSerFD;

        FD_SET(iSerFD, &fdReadSet);

        tTimeOut.tv_sec  = 5;
        tTimeOut.tv_usec = 0;
        iResult= select((iFdMax + 1), &fdReadSet, NULL, NULL, &tTimeOut);
        
        if(0 == iResult)
        {
            continue;
        }
        else if(iResult < 0)
        {
            LOGE(LOG_TAG,"select error happened errno = %d. ", errno);
            continue;
        }
        else
        {
            // 检查客户端Fd是否可读,如果不可读，则断开
            if(!FD_ISSET(iSerFD, &fdReadSet))
            {
                m_ptrSerial->closeSerial();
                LOGD(LOG_TAG,"Serial not read. ");
                continue;
            }
            iResult = processSerialData(fdReadSet);
            if(RTN_SUCCESS != iResult)
            {
                LOGE(LOG_TAG,"Check client data error. ");
                continue;
            }
        }
        //Utils::thrsleep(1000*10);
    }
}

/**
 * 处理串口数据 
 * @param [IN] fdReadSet
 * @param [OUT]
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int DecouplerManager::processSerialData(FdSet &fdReadSet)
{
    int iResult = RTN_FAIL;
    DecouplerData * pDecData;
    //读取出的原始数据
    char achReadBuff[PACK_LEN_MAX]={0};
    //分包之后的数据
    char achDecBuff[PACK_LEN_MAX]={0};
    int  iRecvNum;
    int  iPackLen;
    do
    {
        iResult = m_ptrSerial->recvData(achReadBuff, PACK_LEN_MAX, iRecvNum);
        if(RTN_SUCCESS != iResult)
        {
            break;
        }
        else
        {
            iResult = m_ptrSerial->putDataToDcplr(achReadBuff, iRecvNum);
            if(RTN_SUCCESS != iResult)
            {
                LOGE(LOG_TAG,"Put date into decoupler error. ");
                continue;
            }

            // 正常情况下一次接收一包数据，为了防止客户端数据发送过快一次收到多包数据，加了循环取包
            // 取包处理过程中如果出现异常，不立即返回，丢弃正在处理的包，进行下次取包，直到解包器空
            while(!(m_ptrSerial->isDecouplerEmpty()))
            {
                //获取包描述信息
                iResult = m_ptrSerial->getPackInfo(achDecBuff, PACK_LEN_MAX, iPackLen);
                if(RTN_SUCCESS != iResult)
                {
                    LOGE(LOG_TAG,"Get pack infomation error src module. ");
                    break;   // 若取包异常则跳出，防止发生死循环
                }
                //DebugBuffer(LOG_TAG, "test",  (char*)achDecBuff, iPackLen);
                short pckHeadLen = iPackLen - m_ptrSerial->getPacketContentLen();
                char * bodyData =  achDecBuff+pckHeadLen;
                short bodyLen = m_ptrSerial->getPackageBodyLen();
                //LOGD(LOG_TAG,"get package headlen[%d], bodyLen[%d]", pckHeadLen, bodyLen);
                //DebugBuffer(LOG_TAG, "------------",  (char*)achDecBuff, iPackLen);
                UsbMcuCenter::getInstance()->pushDecoupler(bodyData, bodyLen);
            }
        }
    }while(0);
    return iResult;
}

/**
 * 处理ttyACM设备节点事件
 * @param devName 节点路径
 * @param isAdd 是否是挂载，true挂载 flase 卸载
 */
void DecouplerManager::handleUsbSerialEvents(const char * devName, bool isAdd)
{
    if(isAdd)
    {
        char serialName[SERIAL_PORT_NAME_LEN_MAX] = {0};
        sprintf(serialName, "/dev/%s", devName);
        m_ptrSerial->setSerialName(serialName);
        m_ptrSerial->openSerial();
    }
    else
    {
        m_ptrSerial->closeSerial();
    }
}
