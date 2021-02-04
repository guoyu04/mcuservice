/**
 * UsbMcuCenter.h
 * MCU分包后的协议处理模块，维护一个消息队列，并异步取消息并处理
 * Created by guoyu on 2019/8/27.
 */


#ifndef __USBMCU_CENTER_H__
#define __USBMCU_CENTER_H__

#include <vector>
#include <utils/RefBase.h>
#include "utils/ZMutex.h"
#include "common/ThreadPool.h"
#include "common/Common.h"
#include "common/MsgQueue.h"
#include "decoupler/DecouplerData.h"

class IUsbMcuDataListener : public android::RefBase
{
public:  
    virtual void onRecUsbMcuData(const char * data, int len) = 0;
};

class UsbMcuCenter : public ThreadPool
{
	private:
		/**
		 * construction and destruction
		 **/
		UsbMcuCenter();
		virtual ~UsbMcuCenter();
	public:
		/**
		 * get class instance
		 **/
		static UsbMcuCenter* getInstance();
        int init();

        //线程的运行实体
        virtual void run(TThread * thread);
        //放入分包后的数据
        void pushDecoupler(const char * pchData, short sLen);
        // 设置mcu数据回调监听
        void addUsbMcuDataListener(IUsbMcuDataListener * listener);
        int sendData(char *pchSendData, int iSendLen);
	private:
		static UsbMcuCenter* m_ptrMcuDpl;
        static ZMutex s_Lock;
        //TThread * m_ptrTThread;
        MsgQueue<DecouplerData> m_MsgQueue;
        std::vector< android::sp<IUsbMcuDataListener> >  m_vListener;
};


#endif
