/**
 * SysDevice.cpp
 * FM射频节点实现
 * Created by guoyu on 2019/10/28.
 */
#include <stdio.h>
#include "device/SysDevice.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.device.SysDevice"

#define DEV_CMD_TYPE                0x10                  // 指令功能码
#define DEV_SUB_CMD_TYPE            0x81                  // 子命令

#define DEVID_CAR_SPEAK             0x02000000            // 语音播报标识符ID
#define DEVID_SYS_STATE             0x01234580            // 系统状态标识符ID
//ZMutex SysDevice::s_Lock;

//系统应用包名
//#define PCK_NAME_LUNCHER          "com.pvt.launcher"
#define PCK_NAME_LUNCHER            "com.dfxny.qube"
#define PCK_NAME_MUSIC_QQ           "com.pvetec.musics"
#define PCK_NAME_MUSIC_BLUE         "com.pvetec.bluetooth"
#define PCK_NAME_FM                 "com.pvetec.radio"
#define PCK_NAME_AMAP               "com.autonavi.amapauto"

//导航、蓝牙、FM状态
#define STATE_ZERO                  0b00000011
#define STATE_OPEN                  0x01
#define STATE_CLOSE                 0x02

/**
 * 构造函数
 */
SysDevice::SysDevice()
{
    m_ptrTThread = new TThread();
    m_ptrTThread->start(this);
    m_chTopPck=0x00;
    m_chSysState=0xFF;
    m_chSOSState=0x00;
    m_chGrade=0xFF;
    m_chDestInfo=0xFF;
    m_chOrderInfo=0xFF;
    //char data[10]={0xd5,0xc5,0xd6,0xbe,0xba,0xc0,0xce,0xd2,0xb0,0xae};
    //sendSpeak(data, 10);
    //setGrading(0x08);
    //setDestPerInfo(0x02);
    //setOrderInfo(0x02);
}

/**
 * 析构函数
 */
SysDevice::~SysDevice()
{
    if(NULL != m_ptrTThread)
    {
       delete m_ptrTThread;
    }
}

/**
 * 线程执行体
 */
void SysDevice::run(TThread * thread)
{
    while(1)
    {
        Utils::thrsleep(1000);
        //Utils::thrsleep(20);
        sendSysState();
    }
}

void SysDevice::onRecUsbMcuData(const char * recData, int len)
{
    if(recData == NULL || recData[0] != DEV_CMD_TYPE || len < 2)
    {
        return;
    }

}

//设置当前前台应用
int SysDevice::setTopPackage(std::string &pckName)
{
    if(pckName.length() == 0) return RTN_FAIL;
    if(pckName == PCK_NAME_LUNCHER){
        m_chTopPck = 0x00;
    }else if(pckName == PCK_NAME_MUSIC_QQ){
        m_chTopPck = 0x02;
    }else if(pckName == PCK_NAME_MUSIC_BLUE){
        m_chTopPck = 0x04;
    }else if(pckName == PCK_NAME_FM){
        m_chTopPck = 0x08;
    }else if(pckName == PCK_NAME_AMAP){
        m_chTopPck = 0x10;
    }else{
        m_chTopPck = 0x00;
    }
    return RTN_SUCCESS;
}

/**
 * 设置导航打开状态
 */
int SysDevice::setNavOpenState(int state)
{
    m_chSysState &= ~(STATE_ZERO<<4);
    if(state == DEV_POWER_ON)
    {
        m_chSysState |= STATE_OPEN<<4;
    }
    else
    {
        m_chSysState |= STATE_CLOSE<<4;
    }
     
    LOGD(LOG_TAG,"set navigation state[0x%0x]", m_chSysState);
    return RTN_SUCCESS;
}
/**
 * 设置蓝牙打开状态
 */
int SysDevice::setBleOpenState(int state)
{
    m_chSysState &= ~(STATE_ZERO<<2);
    if(state == DEV_POWER_ON)
    {
        m_chSysState |= STATE_OPEN<<2;
    }
    else
    {
        m_chSysState |= STATE_CLOSE<<2;
    }
     
    LOGD(LOG_TAG,"set ble state[0x%0x]", m_chSysState);
    return RTN_SUCCESS;
}
/**
 * 设置FM打开状态
 */
int SysDevice::setFMOpenState(int state)
{
    m_chSysState &= ~(STATE_ZERO);
    if(state == DEV_POWER_ON)
    {
        m_chSysState |= STATE_OPEN;
    }
    else
    {
        m_chSysState |= STATE_CLOSE;
    }
     
    LOGD(LOG_TAG,"set FM state[0x%0x]", m_chSysState);
    return RTN_SUCCESS;
}

/*
 * 设置人脸认证状态
 */ 
int SysDevice::setFaceState(int state)
{
    m_chSysState &= ~(STATE_ZERO<<6);
    if(state == DEV_POWER_ON)
    {
        m_chSysState |= STATE_OPEN<<6;
    }
    else
    {
        m_chSysState |= STATE_CLOSE<<6;
    }
     
    LOGD(LOG_TAG,"set face state[0x%0x]", m_chSysState);
    return RTN_SUCCESS;
}
/**
 * 发送车机状态
 */
int SysDevice::sendSysState()
{
    int iRet = RTN_FAIL;
    char cmdBuf[15];
    memset(cmdBuf, 0xff, sizeof(cmdBuf));
    char * tmpCmd = cmdBuf;
    *(tmpCmd++) = DEV_CMD_TYPE;//功能码
    *(tmpCmd++) = DEV_SUB_CMD_TYPE;//子命令
    intToBigChar2(DEVID_SYS_STATE, tmpCmd);
    tmpCmd+=CAN_ID_LEN;
    *(tmpCmd++)=0x08;
    *(tmpCmd++)=m_chSysState;//状态：人脸认证、导航、蓝牙、FM
    *(tmpCmd++)=m_chSOSState;//SOS
    //当前前台应用
    *(tmpCmd++)=m_chTopPck;
    *(tmpCmd++)=m_chGrade;//评分等级
    *(tmpCmd++)=m_chDestInfo;//目的地周边优惠信息
    *(tmpCmd++)=m_chOrderInfo;//订单
    //*(tmpCmd++)=0xff;
    //DebugBuffer(LOG_TAG, "send data:", cmdBuf,sizeof(cmdBuf));
    iRet = sendData(cmdBuf, sizeof(cmdBuf));
    LOGD(LOG_TAG,"sendSysState[%d]", iRet);
    return iRet;
}

/**
 * 设置语音播报
 * @param data 播报内容
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int SysDevice::sendSpeak(const char *data, int len)
{
    int iRet = RTN_FAIL;
    char cmdBuf[15];
    int pckCount = (len-1)/CAN_SPEAK_MAX_LEN+1;//最大汉字个数3
    char * tmpCmd;
    
    for(int i=0; i<pckCount; i++)
    {
        memset(cmdBuf, 0xff, sizeof(cmdBuf));
        tmpCmd = cmdBuf;
        *(tmpCmd++) = DEV_CMD_TYPE;//功能码
        *(tmpCmd++) = DEV_SUB_CMD_TYPE;//子命令
        intToBigChar2(DEVID_CAR_SPEAK, tmpCmd);
        tmpCmd+=CAN_ID_LEN;
        *(tmpCmd++)=0x08;
        int sendLen=(i==(pckCount-1))?(len-1)%CAN_SPEAK_MAX_LEN+1:CAN_SPEAK_MAX_LEN;
        *(tmpCmd++)=(char)len;//语音长度
        *(tmpCmd++)=(char)i;//语音序号
        LOGD(LOG_TAG,"sendSpeak: sendlen[%d]", sendLen);
        memcpy(tmpCmd, data+i*CAN_SPEAK_MAX_LEN, sendLen);
        //DebugBuffer(LOG_TAG, "send data:", cmdBuf,sizeof(cmdBuf));
        iRet = sendData(cmdBuf, sizeof(cmdBuf));
    }

    return iRet;
}

/**
 * 设置评分等级
 * @param grade 等级
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int SysDevice::setGrading(int grade)
{
    m_chGrade = (char)grade;
    m_chGrade = (m_chGrade<<4)& 0xFF;
    LOGD(LOG_TAG,"set grading [0x%0x]", m_chGrade);
    return RTN_SUCCESS;
}

/**
 * 目的地周边优惠信息
 * @param type 类型
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int SysDevice::setDestPerInfo(int type)
{
    m_chDestInfo = (char)type;
    m_chDestInfo = (m_chDestInfo<<6) & 0xFF;
    LOGD(LOG_TAG,"set dest info [0x%0x]", m_chDestInfo);
    return RTN_SUCCESS;
}
         
/**
 * 订单信息
 * @param type 类型
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int SysDevice::setOrderInfo(int type)
{
    m_chOrderInfo = (char)type;
    m_chOrderInfo = (m_chOrderInfo<<6) & 0xFF;
    LOGD(LOG_TAG,"set orde type [0x%0x]", m_chOrderInfo);
    return RTN_SUCCESS;
}

/**
 * 一键报警
 * @param key key类型
 * @param state 按下状态
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int SysDevice::setSOSEvent(int key, int state)
{
    if(state == KEY_UP)
    {
        m_chSOSState = 0x00;
    }
    else
    {
        m_chSOSState = (char)key;
    }

    return RTN_SUCCESS;
}
