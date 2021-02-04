/**
 * CarMcuDevice.cpp
 * Tbox数据接收
 * Created by guoyu on 2019/10/28.
 */
#include <stdio.h>
#include <iostream> 
#include "device/CarMcuDevice.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.device.CarMcuDevice"

#define DEV_CMD_TYPE                0x10                  // 指令功能码
#define DEV_SUB_CMD_TYPE            0x01                  // 子命令
#define DEVID_TOBX_ACC              0x01234567            // 车辆状态标识符ID
#define DEVID_TBOX_LONG             0x01234568            // 车辆经度标识符ID
#define DEVID_TBOX_LAT              0x01234566            // 车辆纬度标识符ID
#define DEVID_TBOX_BAT_POW          0x01234569            // 车辆剩余电量标识符ID
#define DEVID_TBOX_BAT_ST           0x01234570            // 车辆电池健康状态标识符ID
#define DEVID_TBOX_GEAR             0x01234573            // 车辆档位状态标识符ID
#define DEVID_TBOX_WIFI             0x01234574            // 车辆wifi账号
#define DEVID_TBOX_WIFI_PWD         0x01234575            // 车辆wifi密码
#define DEVID_TBOX_SPEED            0x01234579            // 车辆车速标识符ID
#define DEVID_TBOX_TOTAL_MIL        0x01234581            // 车辆总里程标识符ID
#define DEVID_TBOX_REM_MIL          0x01234582            // 车辆剩余里程标识符ID
#define DEVID_TBOX_ROT_SPEED        0x01234583            // 车辆转速标识符ID
#define DEVID_TBOX_BAT_VOLTAGE      0x01234584            // 车辆小电瓶电压标识符ID
#define DEVID_TBOX_DEV_ID           0x01234585            // Tbox的设备ID


//ZMutex CarMcuDevice::s_Lock;

/**
 * 构造函数
 */
CarMcuDevice::CarMcuDevice():m_iAccState(ACC_STATE_OFF)
                       ,m_iBatteryPower(0)
                       ,m_iBatteryState(POWER_STATE_NOR)
                       ,m_iCarGear(CAR_GEAR_P)
                       ,m_iCarSpeed(0)
                       ,m_iTotalMileage(0)
                       ,m_iRemMileage(0)
                       ,m_strWifi("")
                       ,m_strWFPwd("")
                       ,m_lDevID(0)
{
   
}

/**
 * 析构函数
 */
CarMcuDevice::~CarMcuDevice()
{
}

/**
 * 处理车辆状态
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processCarState(const char * msg)
{
    const char state = *msg;
    if(state != m_iAccState && m_pListener != NULL)
    {
        LOGD(LOG_TAG,"acc: %d", state);
        m_iAccState = state;
        m_pListener->onRecAccState(state);
    }
}

/**
 * 处理剩余电量
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processBatPower(const char * msg)
{
    char battery = (*(msg+1));
    if(battery != m_iBatteryPower && m_pListener != NULL)
    {
        LOGD(LOG_TAG,"battery: %d", battery);
        m_iBatteryPower = battery;
        m_pListener->onRecBatPowerChange(battery);
    }
}

/**
 * 处理电池状态
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processBatState(const char * msg)
{
    char state = *(msg + 2);
    if(m_iBatteryState != state && m_pListener != NULL)
    {
        LOGD(LOG_TAG,"battery state: %X", state);
        m_iBatteryState = state;
        m_pListener->onRecBatState(state);
    }
}

/**
 * 处理档位状态
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processCarGear(const char * msg)
{
    char gear = *(msg);
    if(m_iCarGear != gear && m_pListener != NULL)
    {
        LOGD(LOG_TAG,"gear: %X", gear);
        m_iCarGear = gear;
        m_pListener->onRecCarGear(gear);
    }
}

/** 处理车速
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processCarSpeed(const char * msg)
{
    short speed = 0;
    char2ToShort(msg, speed);
    if(m_iCarSpeed != speed&& m_pListener != NULL)
    {
        LOGD(LOG_TAG,"speed: %X", speed);
        m_iCarSpeed = speed;
        m_pListener->onRecCarSpeed(speed);
    }
}

/** 处理转速
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processRotateSpeed(const char * msg)
{
    short speed = msg[0]*100 + msg[1];
    //char2ToShort(msg, speed);
    if(m_iRotateSpeed != speed&& m_pListener != NULL)
    {
        m_iRotateSpeed = speed;
        m_pListener->onRecRotateSpeed(speed);
    }
}

/**
 * 处理总里程
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processTotalMileage(const char * msg)
{
    int mileage = 0;
    char2ToInt(msg, mileage);
    if(m_iTotalMileage != mileage && m_pListener != NULL)
    {
        LOGD(LOG_TAG,"total mileage: %d", mileage);
        m_iTotalMileage = mileage;
        m_pListener->onRecTotalMileage(mileage);
    }
}

/**
 * 处理剩余里程
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processRemMileage(const char * msg)
{
    int mileage = 0;
    char2ToInt(msg, mileage);
    if(m_iRemMileage != mileage && m_pListener != NULL)
    {
        LOGD(LOG_TAG,"Remaining mileage: %d", mileage);
        m_iRemMileage = mileage;
        m_pListener->onRecRemMileage(mileage);
    }
}

/**
 * wifi账号
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processWifiAccount(const char * msg)
{
    std::string account(msg);
    account = WIFI_HEAD+account;
    if(m_strWifi != account && m_pListener != NULL)
    {
        LOGD(LOG_TAG,"----processWifiAccount-------%s", account.c_str());
        m_strWifi = account;
        m_pListener->onRecWifiAccount(account);
    }
}
/**
 * wifi密码
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processWifiPwd(const char * msg)
{
    //DebugBuffer(LOG_TAG, "wifi password",  (char*)msg, CAN_DATA_LEN);
    std::string pwd(msg, 0, 4);
    pwd = WIFI_HEAD+pwd;
    if(m_strWFPwd != pwd && m_pListener != NULL)
    {
        LOGD(LOG_TAG,"----processWifiPwd-------%s", pwd.c_str());
        m_strWFPwd = pwd;
        m_pListener->onRecWifiPwd(pwd);
    }

}

/**
 * Tbox ID
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processDevID(const char * msg)
{
    long long devID = ((*(msg+4))&0xff)
                    +((*(msg+3)&0xff)<<8)
                    +((*(msg+2)&0xff)<<16)
                    +((long long)(*(msg+1)&0xff)<<24)
                    +((long long)(*(msg)&0xff)<<32);
   
    if(m_lDevID != devID && m_pListener != NULL)
    {
        m_lDevID = devID;
        LOGD(LOG_TAG,"----processDevID-------%lld", devID);

        ostringstream os;
        os<<devID;
        std::string devIDStr = os.str();
        m_pListener->onRecDevID(devIDStr);
    }
}

/** 处理小电瓶电压
 * @param msg [IN] 消息体
 */
void CarMcuDevice::processBatVoltage(const char * msg)
{
    short voltage = msg[0]*100+msg[1];
    //char2ToShort(msg, voltage);
    if(m_iBatVoltage != voltage&& m_pListener != NULL)
    {
        LOGD(LOG_TAG,"battery voltage: %X", voltage);
        m_iBatVoltage = voltage;
        m_pListener->onRecBatVoltage(voltage);
    }
}


void CarMcuDevice::onRecUsbMcuData(const char * recData, int len)
{
    if(recData == NULL || recData[0] != DEV_CMD_TYPE || len < PACK_BODY_MIN_MIN)
    {
        return;
    }
    const char * temp = recData;
    int canID = getCanID(recData);
    temp += MSG_HEAD_LEN+CAN_ID_LEN;//主命令+子命令+标识ID
    if(*temp != CAN_DATA_LEN) return;
    temp += CAN_LEN_LEN;//长度字段
    LOGD(LOG_TAG,"---rec------[%0X]", canID);
    switch(canID)
    {
        case DEVID_TOBX_ACC:
            processCarState(temp);
            break;
        case DEVID_TBOX_BAT_POW:
            processBatPower(temp);
            break;
        case DEVID_TBOX_BAT_ST:
            processBatState(temp);
            break;
        case DEVID_TBOX_GEAR:
            processCarGear(temp);
            break;
        case DEVID_TBOX_SPEED:
            //DebugBuffer(LOG_TAG, "speed",  (char*)recData, len);
            processCarSpeed(temp);
            break;
        case DEVID_TBOX_ROT_SPEED:
            processRotateSpeed(temp);
            break;
        case DEVID_TBOX_TOTAL_MIL:
            //DebugBuffer(LOG_TAG, "total mileage",  (char*)recData, len);
            processTotalMileage(temp);
            break;
        case DEVID_TBOX_REM_MIL:
            //DebugBuffer(LOG_TAG, "Remaining mileage",  (char*)recData, len);
            processRemMileage(temp);
            break;
        case DEVID_TBOX_LONG:
            break;
        case DEVID_TBOX_LAT:
            //const char * b = temp;
            //long lat = 0;
            //bigChar2ToLong(temp, lat);
            //LOGD(LOG_TAG,"-----------onRecDeviceData: %.6f", Utils::long2Double(lat));
            break;
        case DEVID_TBOX_WIFI:
            processWifiAccount(temp);
            break;
        case DEVID_TBOX_WIFI_PWD:
            processWifiPwd(temp);
            break;
        case DEVID_TBOX_DEV_ID:
            processDevID(temp);
            break;
        case DEVID_TBOX_BAT_VOLTAGE:
            processBatVoltage(temp);
            break;
    }
}

/**
 * 设置回调监听
 * @param listener 回调监听
 */
void CarMcuDevice::setCarMcuDevListener(ICarMcuDevListener * listener)
{
    m_pListener = listener;
    //TODO test
   /*char buf[5];
   buf[0]=0x04;
   buf[1]=0xA8;
   buf[2]=0x36;
   buf[3]=0xE8;
   buf[4]=0xF1;
   processDevID(buf);*/
}

/**
 * 获取车辆状态
 * @return 车辆状态
 */
char CarMcuDevice::getAccState()
{
    LOGD(LOG_TAG,"acc: %X", m_iAccState);
    return m_iAccState;
}

/**
 * 获取剩余电量
 * @return 电量
 */
char CarMcuDevice::getBatteryPower()
{
    LOGD(LOG_TAG,"battery: %X", m_iBatteryPower);
    return m_iBatteryPower;
}

/**
 * 获取电池状态
 * @return 电池状态
 */
char CarMcuDevice::getBatteryState()
{
    return m_iBatteryState;
}

/**
 * 获取档位状态
 * @return 档位状态
 */
char CarMcuDevice::getCarGear()
{
    return m_iCarGear;
}

/**
 * 获取车速
 * @return 车速
 */
int CarMcuDevice::getCarSpeed()
{
    return m_iCarSpeed;
}

/**
 * 获取转速
 * @return 转速
 */
int CarMcuDevice::getRotateSpeed()
{
    return m_iRotateSpeed;
}

/**
 * 获取总里程
 * @return 总里程
 */
int CarMcuDevice::getTotalMileage()
{
    return m_iTotalMileage;
}

/**
 * 获取剩余里程
 * @return 剩余里程
 */
int CarMcuDevice::getRemMileage()
{
    return m_iRemMileage;
}

/**
 * 获取wifi账号
 * @return wifi账号
 */
std::string CarMcuDevice::getWifiAccount()
{
    return m_strWifi;
}

/**
 * 获取wifi密码
 * @return wifi密码
 */
std::string CarMcuDevice::getWifiPwd()
{
    return m_strWFPwd;
}

/**
 * 获取device id
 * @return device id
 */
std::string CarMcuDevice::getDeviceID()
{
    ostringstream os;
    os<<m_lDevID;
    std::string devIDStr = os.str();
    return devIDStr;
}

/**
 * 获取小电瓶电压
 * @return 小电瓶电压
 */
int CarMcuDevice::getBatVoltage()
{
    return m_iBatVoltage;
}
