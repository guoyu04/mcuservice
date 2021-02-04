/**
 * ThreadPool.cpp
 * 线程池实现类
 * Created by guoyu on 2019/8/27.
 */
#include "common/ThreadPool.h"
#include "utils/ZLog.h"
#include <stdio.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "mcu.common.ThreadPool"

ThreadPool::ThreadPool(int threadCount)
{
    m_iThreadCount = threadCount;
    m_aptrThread = NULL;
}

/**
 * 开始执行线程
 */
int ThreadPool::start()
{
    int ret = 0;

    if (NULL != m_aptrThread || 1 > m_iThreadCount)
    {
        LOGD(LOG_TAG, "start thread error, m_aptrThread = %p, m_iThreadCount = %d", m_aptrThread, m_iThreadCount);
        ret = -1;
    } 
    else if (NULL != (m_aptrThread = new TThread[m_iThreadCount]))
    {
        for (int i = 0; i < m_iThreadCount; i++)
        {
            //m_aptrThread[i].set_thread_index(i);
            if (0 != (ret = m_aptrThread[i].start(this)))
            {
                break; 
            }
        }
    }

    return ret;
}

void ThreadPool::wait()
{
    if (NULL != m_aptrThread)
    {
        for (int i = 0; i < m_iThreadCount; i++)
        {
            m_aptrThread[i].join();
        }
    } 
}

/**
 * 设置线程池线程个数
 * @parma count 线程池线程个数
 */ 
void ThreadPool::setThreadCount(int count)
{
    if (NULL != m_aptrThread)
    {
        LOGD(LOG_TAG,"runnning, cannot set thread_count\n");
    }
    else
    {
        m_iThreadCount = count;
    }
}

ThreadPool::~ThreadPool()
{
    if (NULL != m_aptrThread)
    {
        delete[] m_aptrThread;
        m_aptrThread = NULL;
    }
}

