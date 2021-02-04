/**
 * RemoteServiceList.h
 * 远程服务列表，会维护一个死亡通知列表 
 * Created by guoyu on 2019/10/30.
 */

#ifndef __REMOTE_SERVICE_LIST_H__
#define __REMOTE_SERVICE_LIST_H__

#include <binder/IBinder.h>
#include <utils/RefBase.h>
#include <binder/Parcel.h>
#include <vector>
#include <string>
#include "utils/ZMutex.h"


typedef android::sp<android::IBinder> SpBinder;
typedef android::wp<android::IBinder> WpBinder;

class RemoteServiceList : public android::RefBase
{
    friend class DeathNotifier;
    friend class RemoteServiceData;

    /**
     * 死亡通知
     */
    class DeathNotifier: public android::IBinder::DeathRecipient
    {
        public:
            DeathNotifier(android::sp<RemoteServiceList> remoteLsit);
            virtual ~DeathNotifier();
            virtual void binderDied(const WpBinder& who);
        private:
            DeathNotifier(){};
        private:
            android::sp<RemoteServiceList> m_ptrRemoteList;
    };

    /**
     * 远程服务数据存储，构造时注册死亡通知，析构时注销
     */
    class RemoteServiceData : public android::RefBase
    {
        public:
            RemoteServiceData(SpBinder remote, android::sp<DeathNotifier>  death);
            virtual ~RemoteServiceData();
        public:
            android::sp<android::IBinder> m_remoteSer;
            android::sp<DeathNotifier> m_deathNotifier;
    };

	public:
		RemoteServiceList();
		virtual ~RemoteServiceList();
        void addList(SpBinder remote);//添加远程服务
        void removeList(SpBinder remote);//移除远程服务
        void sendData2Remote(int code, android::Parcel & data);//发送数据到远程服务
    private:
        std::vector< android::sp<RemoteServiceData> >  m_vRemotes;
        ZMutex m_ListLock;

};

#endif
