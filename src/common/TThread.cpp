/**
 * TThread.cpp
 * 线程类
 * Created by guoyu on 2019/8/27.
 */
#include <unistd.h>
#include "common/TThread.h"
#include "utils/ZLog.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "mcu.common.TThread"

TThread::TThread()
{
    m_tid = 0;
    m_runnable = NULL;
}

TThread::~TThread()
{
}

/**
 * 获取线程回调 
 *
 * @return  Runnable* 
 **/
Runnable * TThread::getRunnable()
{
    return m_runnable;
}

/**
 * 开始执行线程
 * @param 回调
 */
bool TThread::start(Runnable * runnable)
{
    m_runnable = runnable;
    return pthread_create(&m_tid, NULL, TThread::threadFunc, this) == 0;
}

/**
 * 获取线程ID
 */
pthread_t TThread::getThreadID()
{
    return m_tid;
}


/**
 * 等待线程直至退出
 */
void TThread::join()
{
    if (m_tid > 0)
    {
        pthread_join(m_tid, NULL);
    }
}

/**
 * 获取执行方法的指针
 */
void * TThread::threadFunc(void * args)
{
    TThread * pThread = static_cast<TThread *>(args);
    if (pThread->getRunnable())
    {
        pThread->getRunnable()->run(pThread);
    }
    return NULL; 
}
