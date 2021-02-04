/**
 * CarMcuDevice.h
 * Tbox数据接收头文件 
 * Created by guoyu on 2019/12/27.
 */

#ifndef __CAR_MCU_DEVICE_H__
#define __CAR_MCU_DEVICE_H__
#include "device/BaseUsbDevice.h"

class ICarMcuDevListener : public virtual android::RefBase
{
public:  
    virtual void onRecAccState(char state) = 0;//车辆状态
    virtual void onRecBatPowerChange(char battery) = 0;//剩余电量
    virtual void onRecBatState(char state) = 0;//电池状态
    virtual void onRecCarGear(char gear) = 0;//档位状态
    virtual void onRecCarSpeed(int speed) = 0;//车速
    virtual void onRecRotateSpeed(int speed) = 0;//转速
    virtual void onRecTotalMileage(int mileage) = 0;//总里程
    virtual void onRecRemMileage(int mileage) = 0;//剩余里程
    virtual void onRecWifiAccount(std::string account) = 0;//wifi账号
    virtual void onRecWifiPwd(std::string pwd) =0;//wifi密码
    virtual void onRecDevID(std::string devID) =0;//Tbox ID
    virtual void onRecBatVoltage(int voltage) = 0; //小电瓶电压
};

class CarMcuDevice : public BaseUsbDevice {
	public:
		CarMcuDevice();
		virtual ~CarMcuDevice();
        //设置回调监听
        void setCarMcuDevListener(ICarMcuDevListener * listener);
        char getAccState();//获取车辆状态
        char getBatteryPower();//获取剩余电量
        char getBatteryState();//获取电池状态
        char getCarGear();//获取档位状态
        int getCarSpeed();//获取车速
        int getRotateSpeed();//获取转速
        int getTotalMileage();//获取总里程
        int getRemMileage();//获取剩余里程
        std::string getWifiAccount();//获取wifi账号
        std::string getWifiPwd();//获取wifi密码
        std::string getDeviceID();//获取device id
        int getBatVoltage(); //获取小电瓶电压
	private:
        virtual void onRecUsbMcuData(const char * data, int len);
        void processCarState(const char * msg);//车辆状态
        void processBatPower(const char * msg);//剩余电量
        void processBatState(const char * msg);//电池状态
        void processCarGear(const char * msg);//档位状态
        void processCarSpeed(const char * msg);//车速
        void processRotateSpeed(const char * msg);//转速
        void processTotalMileage(const char * msg);//总里程
        void processRemMileage(const char * msg);//剩余里程
        void processWifiAccount(const char * msg);//wifi账号
        void processWifiPwd(const char * msg);//wifi密码
        void processDevID(const char * msg);//Tbox ID
        void processBatVoltage(const char * msg);//小电瓶电压
    private:
        android::sp<ICarMcuDevListener> m_pListener;
        char m_iAccState;//车辆状态
        char m_iBatteryPower;//剩余电量
        char m_iBatteryState;//电池健康状态
        char m_iCarGear;//档位状态
        short m_iCarSpeed;//车速
        short m_iRotateSpeed;//转速
        int m_iTotalMileage;//总里程
        int m_iRemMileage;//剩余里程
        std::string m_strWifi;//wifi账号
        std::string m_strWFPwd;//wifi密码
        long long m_lDevID;//TboxID
        int m_iBatVoltage;//小电瓶电压
        //static ZMutex s_Lock;
};


#endif
