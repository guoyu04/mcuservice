/**
 * SysDevice.h
 * FM射频节点头文件 
 * Created by guoyu on 2019/12/27.
 */

#ifndef __SYS_DEVICE_H__
#define __SYS_DEVICE_H__
#include "device/BaseUsbDevice.h"

class SysDevice :public Runnable, public BaseUsbDevice {
	public:
		SysDevice();
		virtual ~SysDevice();

        int sendSpeak(const char *data, int len);//设置语音播报
        //设置当前前台应用
        int setTopPackage(std::string &pckName);
        //设置导航打开状态
        int setNavOpenState(int state);
        //设置蓝牙打开状态
        int setBleOpenState(int state);
        //设置FM打开状态
        int setFMOpenState(int state);

        //设置人脸认证状态
        int setFaceState(int state);
        //设置评分等级
        int setGrading(int grade);
        //目的地周边优惠信息
        int setDestPerInfo(int type);
        //订单信息
        int setOrderInfo(int type);
        //一键报警
        int setSOSEvent(int key, int state);
    protected:
        //线程的运行实体
        virtual void run(TThread * thread);
	private:
        virtual void onRecUsbMcuData(const char * data, int len);
        int sendSysState();//发送系统状态
    private:
        TThread * m_ptrTThread;
        char m_chTopPck;
        char m_chSysState;
        char m_chSOSState;//SOS
        char m_chGrade;//评分等级
        char m_chDestInfo;//目的地周边优惠信息
        char m_chOrderInfo;//订单信息
        //android::sp<ISysDevListener> m_pListener;
        //static ZMutex s_Lock;
};


#endif
