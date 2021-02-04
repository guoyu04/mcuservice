/**
 * RemoteServiceList.cpp
 * 远程服务列表，会维护一个死亡通知列表 
 * Created by guoyu on 2019/10/30.
 */
#include "service/RemoteServiceList.h"
#include "utils/ZLog.h"


#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.service.RemoteServiceList"

/**
 * 构造函数
 */
RemoteServiceList::RemoteServiceList()
{
}

/**
 * 析构函数
 */
RemoteServiceList::~RemoteServiceList()
{
    ZMutex::Autolock lock(m_ListLock);
    for(std::vector< android::sp<RemoteServiceData> >::iterator it = m_vRemotes.begin();
            it != m_vRemotes.end(); it++)
	{
        android::sp<RemoteServiceData> remoteData = *it;
        remoteData->m_remoteSer->unlinkToDeath(remoteData->m_deathNotifier);
	} 
}

/**
 * 添加远程服务
 * @param remote 代理句柄
 */
void RemoteServiceList::addList(SpBinder remote)
{
    LOGI( LOG_TAG, "RemoteServiceList : addList remote proxy  %p, size[%zu]",remote.get(), m_vRemotes.size());
    ZMutex::Autolock lock(m_ListLock);
    for(std::vector< android::sp<RemoteServiceData> >::iterator it = m_vRemotes.begin(); it != m_vRemotes.end(); it++)
	{
        android::sp<RemoteServiceData> remoteData = *it;

		if(remoteData->m_remoteSer.get() == remote.get()) 
		{
			LOGD(LOG_TAG, "proxy [%p] already register, delete it before registering.\n ", remote.get());
			m_vRemotes.erase(it);
            break;
		}
	}
    //death notifier
    android::sp<DeathNotifier> death = new DeathNotifier(this);
	//构造数据类型
    android::sp<RemoteServiceData> data = new RemoteServiceData(remote, death);
    m_vRemotes.push_back(data);
    LOGI( LOG_TAG, "CarFMService : registRemoteProxy remote proxy  %p",remote.get());
}

/**
 * 移除远程服务
 * @param remote 代理句柄
 */
void RemoteServiceList::removeList(SpBinder remote)
{
    ZMutex::Autolock lock(m_ListLock);
    LOGI( LOG_TAG, "removeList %p, size[%zu]", remote.get(), m_vRemotes.size());
    for(std::vector< android::sp<RemoteServiceData> >::iterator it = m_vRemotes.begin(); 
                    it != m_vRemotes.end(); it++)
	{
        android::sp<RemoteServiceData> remoteData = *it;
        LOGI( LOG_TAG, "---- %p", remoteData->m_remoteSer.get());
		if(remoteData->m_remoteSer.get() == remote.get()) 
		{
			m_vRemotes.erase(it);
            LOGD(LOG_TAG, "proxy [%p] remove for list.\n ", remote.get());
            break;
		}
	}
}

/**
 * 发送数据到远程服务
 * @param remote 代理句柄
 */
void RemoteServiceList::sendData2Remote(int code, android::Parcel & data)
{
    ZMutex::Autolock lock(m_ListLock);
    LOGI( LOG_TAG, "sendData2Remote, code[%d]", code);
    for(std::vector< android::sp<RemoteServiceData> >::iterator it = m_vRemotes.begin(); 
                    it != m_vRemotes.end(); it++)
	{
        android::sp<RemoteServiceData> remoteData = *it;
        LOGI( LOG_TAG, "---- %p", remoteData->m_remoteSer.get());
        android::Parcel reply;
        remoteData->m_remoteSer->transact(code, data, &reply);
	}
}

//================================================
//death notifier

RemoteServiceList::DeathNotifier::DeathNotifier(android::sp<RemoteServiceList> remoteLsit) : m_ptrRemoteList(remoteLsit)
{
    //
}

/**
 * 死亡通知回调
 * @param who 死亡服务代理
 */
void RemoteServiceList::DeathNotifier::binderDied(const WpBinder& who __unused) 
{
    LOGI( LOG_TAG, "binderDied %p", who.unsafe_get()); 
    //SpVector::iterator iter=std::find(m_ptrRemoteList->m_vRemotes.begin(),m_ptrRemoteList->m_vRemotes.end(),who);
    //m_ptrRemoteList->m_vRemotes.erase(iter);
    m_ptrRemoteList->removeList(who.promote());
    
}

RemoteServiceList::DeathNotifier::~DeathNotifier()
{
    LOGI( LOG_TAG, "~DeathNotifier");
    /*if (sService != 0) {
        IInterface::asBinder(sService)->unlinkToDeath(this);
    }*/
}

//==================================================
//远程服务数据存储，构造时注册死亡通知，析构时注销
RemoteServiceList::RemoteServiceData::RemoteServiceData(SpBinder remote, android::sp<RemoteServiceList::DeathNotifier>  death)
                                        :m_remoteSer(remote),m_deathNotifier(death) 
{
    m_remoteSer->linkToDeath(m_deathNotifier);
}

RemoteServiceList::RemoteServiceData::~RemoteServiceData()
{
    LOGI( LOG_TAG, "~RemoteServiceData");
    m_remoteSer->unlinkToDeath(m_deathNotifier);
}
