/**
 * ThreadPool.h
 * 线程池实现类
 * Created by guoyu on 2019/8/27.
 */
#ifndef  __THREAD_POOL_H_
#define  __THREAD_POOL_H_

#include "common/TThread.h"

class ThreadPool : public Runnable
{
    public:
        ThreadPool(int thread_count = 1);

        //线程回调        
        virtual void run(TThread* thread) = 0;
        //启动线程池
        int start();
        void wait();
        //线程池线程个数
        void setThreadCount(int thread_count);

        virtual ~ThreadPool();
    protected:
        //线程个数
        int m_iThreadCount;
        //线程数组
        TThread* m_aptrThread;
};
#endif  //__THREAD_POOL_H_

