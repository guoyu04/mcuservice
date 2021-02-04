/**
 * DecouplerManager.h
 * MCU协议解析，此模块维护一个UsbSerial或者一系列UsbSerial，负责mcu协议的解析
 * Created by guoyu on 2019/8/27.
 */


#ifndef __DECOUPLER_MANAGER_H__
#define __DECOUPLER_MANAGER_H__

#include <vector>
#include "utils/Lock.h"
#include "common/TThread.h"
#include "common/Common.h"

using namespace std;
class TThread;
class UsbSerial;
class Decoupler;

class DecouplerManager : public Runnable
{
	public:
		/**
		 * get class instance
		 **/
		static DecouplerManager* getInstance();
        int init();
        int sendDataToSerial(char *pchSendData, int iSendLen);//发送数据
        void handleUsbSerialEvents(const char * devName, bool isAdd);//处理ttyACM设备节点事件 
    private:
		/**
		 * construction and destruction
		 **/
        DecouplerManager();
        virtual ~DecouplerManager();
        int processSerialData(FdSet &fdReadSet);  // 处理串口数据
        Decoupler * makeDecoupler(char * prefix, int prefixLen);
        //线程的运行实体
        virtual void run(TThread * thread);
	private:
		static DecouplerManager* m_ptrMcuDpl;
		static CLock m_Lock;
        UsbSerial* m_ptrSerial;
        TThread * m_ptrTThread;
};


#endif
