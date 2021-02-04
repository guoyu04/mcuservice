/**
 * CLock.h
 * CLock 互斥锁类
 * Created by guoyu on 2019/8/27.
 */

#ifndef _Lock_H
#define _Lock_H

#include <pthread.h>

class CLock
{
public:
	CLock();
	~CLock();

	void Lock() const;
	void Unlock() const;

private:
	mutable pthread_mutex_t m_mutex;
};

#endif

