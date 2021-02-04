/**
 * CarSystemService.h
 * android系统状态模块：负责接收acc、重启等状态
 * Created by guoyu on 2019/10/27.
 */
#ifndef CAR_SYS_SERVICE_H_
#define CAR_SYS_SERVICE_H_
#include <stdlib.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <string>
#include <binder/IBinder.h>
#include "service/ICarSystemService.h"
#include "common/Common.h"
#include "utils/ZMutex.h"
#include "service/RemoteServiceList.h"
#include "common/ThreadPool.h"
#include "common/MsgData.h"
#include "common/MsgQueue.h"
#include "device/SysDevice.h"
#include "device/CarMcuDevice.h"
#include "device/McuIODevice.h"

class CarSystemService: public BnCarSystemService, public ThreadPool, public ICarMcuDevListener, public IMcuIODevListener
{
    public:
        virtual ~CarSystemService();

        static CarSystemService *getInstance();

        void init();
        virtual void onRecAccState(char state);//车辆状态
        virtual void onRecBatPowerChange(char battery);//剩余电量
        virtual void onRecBatState(char state);//电池状态
        virtual void onRecCarGear(char gear);//档位状态
        virtual void onRecCarSpeed(int speed);//车速
        virtual void onRecRotateSpeed(int speed);//转速
        virtual void onRecTotalMileage(int mileage);//总里程
        virtual void onRecRemMileage(int mileage);//剩余里程
        virtual void onRecWifiAccount(std::string account);//wifi账号
        virtual void onRecWifiPwd(std::string pwd);//wifi密码
        virtual void onRecDevID(std::string devID); //TBox ID
        virtual void onRecBatVoltage(int voltage); //小电瓶电压
    protected:
        //注册代理
        virtual int32_t registRemoteProxy(sp<IBinder> proxy, std::string &pckName);
        //注销代理
        virtual int32_t unRegistRemoteProxy(sp<IBinder> proxy, std::string &pckName);
        //发送语音
        virtual int32_t sendSpeak(const char * data, int len);
        //设置当前前台应用
        virtual int32_t setTopPackage(std::string pckName);
        //设置导航打开状态
        virtual int setNavOpenState(int state);
        //设置蓝牙打开状态
        virtual int setBleOpenState(int state);
        //设置FM打开状态
        virtual int setFMOpenState(int state);

        //设置人脸认证状态
        virtual int setFaceState(int state);
        //设置评分等级
        virtual int setGrading(int grade);
        //目的地周边优惠信息
        virtual int setDestPerInfo(int type);
        //订单信息
        virtual int setOrderInfo(int type);

        virtual int getAccState();//获取车辆状态
        virtual int getBatteryPower();//获取剩余电量
        virtual int getBatteryState();//获取电池状态
        virtual int getCarGear();//获取档位状态
        virtual int getCarSpeed();//获取车速
        virtual int getRotateSpeed();//获取转速
        int getTotalMileage();//获取总里程
        virtual int getRemMileage();//获取剩余里程
        virtual std::string getWifiAccount();//获取wifi账号
        virtual std::string getWifiPwd();//获取wifi密码
        virtual std::string getDeviceID();//获取device id
        virtual int getBatVoltage();//获取小电瓶电压
        virtual void onRecKeyEvent(char key, char state);

        //线程的运行实体
        virtual void run(TThread * thread);
        //添加消息到消息队列
        void addMessage(int code, int status);
    private:
        CarSystemService();
    private:
        static CarSystemService *s_ptrService;
        //远程服务列表
        static android::sp<RemoteServiceList>  s_vBinders;
        static ZMutex   s_ServiceLock; 
        //消息队列
        MsgQueue<MsgData> m_MsgQueue;
    private:
        SysDevice *m_ptrSysDev;
        CarMcuDevice * m_ptrMcuDev;
        McuIODevice * m_ptrIODev;
        //BtnUsbDevice *m_ptrBtnDev;
};
#endif
