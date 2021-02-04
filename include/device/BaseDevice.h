/**
 * BaseDevice.h
 *
 * 设备节点类，此模块维护一个Decoupler，一个设备节点对应一种类型的协议格式
 * 维护一个线程，从节点读取数据
 * Created by guoyu on 2019/10/27.
 */

#ifndef __BASE_DEVICE_H__
#define __BASE_DEVICE_H__
#include <utils/RefBase.h>
#include "common/Common.h"
#include "common/TThread.h"


/*----------------------------------类定义-----------------------------------*/

class BaseDevice : public Runnable,public android::RefBase
{
public:
    BaseDevice();
    explicit BaseDevice(std::string devName);
    virtual ~BaseDevice();
    int openDevice(bool isCheckMcu = true);  // 打开设备节点
    int closeDevice();   // 断开设备节点
    bool isDeviceOpen(); //设备是否打开
    void setDeviceName(std::string devName);//设置设备节点路径
    int sendData(char *pchSendData, int iSendLen);   // 从设备节点发送数据
    int recvData(char *pchRecvBuff, int iRecvBuffLen, int &iRecvLen);  // 从设备节点接收数据
    int getOpenStatus();  // 获取设备节点打开状态
    int getOpenFd();  // 获取设备节点打开的描述符
    //查询指令
    int checkMcuStatus();
    //尝试打开设备节点
    int tryOpenDevice();
protected:
    //线程的运行实体
     virtual void run(TThread * thread);
     virtual void onRecDeviceData(char * redData, int recLen);
private:
    int writeN(int iFd, char *pchBuff, int iWriteLen);                    // 写n个byte到描述符
    int readN(int iFd, char *pchBuff, int iBuffLen, int &iReadLen);       // 从描述符读n个byte
private:
    std::string m_strDevName;
    int m_iOpenState;  // 设备节点打开状态
    int m_iFD;     // 设备节点打开Fd
    int m_iBaud;  // 设备节点波特率
    TThread * m_ptrTThread;
};


#endif /* __SERIAL_H__ */




