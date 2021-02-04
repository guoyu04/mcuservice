/**
 * UsbSerial.h
 *
 * 串口类，此模块维护一个Decoupler，一个串口对应一种类型的协议格式
 * Created by guoyu on 2019/8/27.
 */

#ifndef __USBSERIAL_H__
#define __USBSERIAL_H__
#include "common/Common.h"

/*----------------------------------宏定义-----------------------------------*/
#define SERIAL_PORT_NAME_LEN_MAX             (0x80)   // 串口名字长度最大值(128)




/*----------------------------------类定义-----------------------------------*/
class Decoupler;

class UsbSerial
{
public:
    UsbSerial();
    UsbSerial(const char *pchSrlPrtNm, int iBaud, Decoupler *decoupler);
    ~UsbSerial();
    int openSerial();  // 打开串口
    int closeSerial();   // 断开串口
    void setSerialName(const char * srlPrtNm);
    int sendData(char *pchSendData, int iSendLen);   // 从串口发送数据
    int recvData(char *pchRecvBuff, int iRecvBuffLen, int &iRecvLen);  // 从串口接收数据
    int getConnectStatus();  // 获取串口连接状态
    int getConnectFd(int &iFd);  // 获取串口连接的描述符
    //int RegSerialPortCmdFunc(char chCmd, PSrlPrtFunc pfnSrlPrtFunc);  // 注册串口命令字处理函数
    //int ProUsbSerialPortData(char chCmd, char *pchData, int iDataLen);  // 处理串口数据
    //分包数据相关接口
    int putDataToDcplr(char *pchData, int iDataLen);  // 将数据放入分包器
    int getPackInfo(char *pchPackData, int iPackBufSize, int &iPackLen);  // 获取分包数据
    short getPacketContentLen();//获取包内容的实际长度:除去分隔符、包长度标识后的长度
    short getPackageBodyLen();
    //int RegCnctSucsFunc(PSrlCntScsFunc pfnCntScsFunc);  // 注册串口连接成功处理函数
    bool isDecouplerEmpty();  // 分包器是否为空

private:
    int configSerial(int iBaud);   // 配置串口
    int writeN(int iFd, char *pchBuff, int iWriteLen);                    // 写n个byte到描述符
    int readN(int iFd, char *pchBuff, int iBuffLen, int &iReadLen);       // 从描述符读n个byte
private:
    //CCmdProc<char, PSrlPrtFunc, char*, int> m_cSrlPtProc;    // 串口命令处理类
    char m_achSerialName[SERIAL_PORT_NAME_LEN_MAX];             // 串口名
    Decoupler *m_ptrDecoupler;          // 分包器实例
    int m_iFD;     // 串口连接Fd
    int m_iConnectState;  // 串口连接状态
    int m_iBaud;  // 串口波特率
};


#endif /* __SERIAL_H__ */




