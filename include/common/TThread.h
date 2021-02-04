/**
 * TThread.h
 * 线程类
 * Created by guoyu on 2019/8/27.
 */
#ifndef _TTHREAD_H_
#define _TTHREAD_H_

#include <iostream>
#include <pthread.h>

using namespace std;

class TThread;

class Runnable
{
    public:
        virtual void run(TThread * thread) = 0;
        virtual ~ Runnable()
        {
        };
};

class TThread
{
    public:
        //构造函数
        TThread();
        virtual ~TThread();
        //开始执行线程
        bool start(Runnable * runnable);
        //获取线程ID
        pthread_t getThreadID();
        //等待线程直至退出
        void join();
        //获取线程回调 
        Runnable * getRunnable();
    private:
         Runnable *m_runnable;
        //当前线程的线程ID
        pthread_t m_tid;
        //not implement
        TThread(const TThread& );
        TThread& operator=(const TThread& );
        //获取执行方法的指针
        static void * threadFunc(void * args);
};

#endif
