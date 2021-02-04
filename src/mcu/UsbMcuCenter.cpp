/**
 * UsbMcuCenter.cpp
 * MCU分包后的协议处理模块
 * 维护一个消息队列，如果消息队列有消息，则从消息队列读取消息，回调给监听者
 * Created by guoyu on 2019/8/27.
 */
#include "mcu/UsbMcuCenter.h"
#include "utils/ZLog.h"
#include "decoupler/DecouplerManager.h"

#include <string>
using std::string;

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.decoupler.UsbMcuCenter"

UsbMcuCenter* UsbMcuCenter::m_ptrMcuDpl = NULL;
ZMutex UsbMcuCenter::s_Lock;

UsbMcuCenter::UsbMcuCenter()
{
    //m_ptrTThread = new TThread();
}

UsbMcuCenter::~UsbMcuCenter()
{
   /*if(NULL != m_ptrTThread)
   {
       delete m_ptrTThread;
   }*/
}

UsbMcuCenter* UsbMcuCenter::getInstance()
{
    if( NULL == m_ptrMcuDpl )
	{
	    ZMutex::Autolock lock(s_Lock);	
		if( NULL == m_ptrMcuDpl ) 
		{
			 m_ptrMcuDpl = new UsbMcuCenter();
		}

	}

	return m_ptrMcuDpl;
}

/**
 * 初始化
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int UsbMcuCenter::init()
{
    int iResult = RTN_FAIL;
    do
    {
        setThreadCount(2);
        bool isSucess = start();
        iResult = isSucess ? RTN_SUCCESS : RTN_FAIL;
    }while(0);
    return iResult;
}

/**
 * 发送数据
 * @param [IN] pchSendData 发送数据缓存
 * @param [IN] iSendLen 发送数据长度
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int UsbMcuCenter::sendData(char *pchSendData, int iSendLen)
{
    return DecouplerManager::getInstance()->sendDataToSerial(pchSendData, iSendLen);
}

/**
 * 设置mcu数据回调监听
 * @param listener 回调监听
 */
void UsbMcuCenter::addUsbMcuDataListener(IUsbMcuDataListener * listener)
{
    m_vListener.push_back(listener);
}

/**
 * 放入分包后的数据
 * @param data 分包后的数据体
 */ 
void UsbMcuCenter::pushDecoupler(const char * pchData, short sLen)
{
    m_MsgQueue.putMsg(new DecouplerData(pchData, sLen));
}

void UsbMcuCenter::run(TThread * thread)
{
    LOGD(LOG_TAG,"run");
    while(1)
    {
        DecouplerData* data =  m_MsgQueue.getMsg();

        char * chData = NULL;
        short sLen = 0;
        data->getData(&chData, &sLen);
        //处理完删除
        for(std::vector< android::sp<IUsbMcuDataListener> >::iterator it = m_vListener.begin(); 
                it != m_vListener.end(); it++)
        {
            android::sp<IUsbMcuDataListener> pListener = *it;
            pListener->onRecUsbMcuData(chData, sLen);
        }
        delete data;
    }
}
