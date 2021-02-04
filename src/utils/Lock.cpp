/**
 * CLock.cpp
 * CLock 互斥锁类
 * Created by guoyu on 2019/8/27.
 */
#include "utils/Lock.h"


/**
 * 动态方式初始化互斥锁
 */
CLock::CLock()
{
	pthread_mutex_init(&m_mutex, NULL);
}

/**
 * 注销互斥锁
 */
CLock::~CLock()
{
	pthread_mutex_destroy(&m_mutex);
}

/**
 *确保拥有互斥锁的线程对被保护资源的独自访问
 */
void CLock::Lock() const
{
	pthread_mutex_lock(&m_mutex);
}

/**
 * 释放当前线程拥有的锁，以使其它线程可以拥有互斥锁，对被保护资源进行访问
 */
void CLock::Unlock() const
{
	pthread_mutex_unlock(&m_mutex);
}
