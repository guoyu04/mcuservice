/**
 * MsgQueue.h
 * 消息队列头文件 
 * Created by guoyu on 2019/8/27.
 */
#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

template <class T1>
class MsgNode 
{
    public:
        MsgNode(T1* data):m_Data(data), m_ptrNext(NULL)
        {
        }
        ~MsgNode()
        {
        }

        MsgNode *getNext(void)
        {
            return this->m_ptrNext;
        }
        void setNext(MsgNode *msg)
        {
            this->m_ptrNext = msg;
        }
        T1* getData()
        {
            return this->m_Data;
        }
    private:
        T1* m_Data;
        MsgNode *m_ptrNext;
};

template <class T>
class MsgQueue
{
    public:
        MsgQueue();
        ~MsgQueue();
        T * getMsg();
        void putMsg(T * msg);

    private:
        MsgNode<T> *m_ptrHead;
        MsgNode<T> *m_ptrTail;
        pthread_mutex_t m_pLock;
        pthread_cond_t m_pCond; 
};

//===================================================================//
/**
 * 构造函数
 */
template <class T> 
MsgQueue<T>::MsgQueue():m_ptrHead(NULL), m_ptrTail(NULL)
{
    pthread_mutex_init(&(this->m_pLock), NULL);
    pthread_cond_init(&(this->m_pCond), NULL);
}


/**
 * 析构函数
 */
template <class T>
MsgQueue<T>::~MsgQueue()
{
}


/**
 * 往队列缓冲区中放数据
 * @return 消息内容 
 */
template <class T>
T * MsgQueue<T>::getMsg()
{
    T* data;
    MsgNode<T> *msg = NULL;
    pthread_mutex_lock(&this->m_pLock);
    while (NULL == this->m_ptrHead)
    {
        pthread_cond_wait(&this->m_pCond, &this->m_pLock);
    }
    msg = this->m_ptrHead;
    this->m_ptrHead = this->m_ptrHead->getNext();
    if (NULL == this->m_ptrHead)
    {
        this->m_ptrTail = NULL;
    }
    pthread_mutex_unlock(&this->m_pLock);

    data = msg->getData();
    delete msg;
    return data;
}

/**
 * 往队列缓冲区中放数据
 * @param [IN] msg 消息内容
 */
template <class T>
void MsgQueue<T>::putMsg(T * data)
{
    if (NULL == data) return;
    MsgNode<T>* msg = new MsgNode<T>(data);
    if(NULL == msg) return;
    pthread_mutex_lock(&this->m_pLock);
    if (NULL == this->m_ptrHead)
    {
        this->m_ptrHead = msg;
        this->m_ptrTail = msg;
        pthread_cond_signal(&this->m_pCond);
    }
    else
    {
        this->m_ptrTail->setNext(msg);
        this->m_ptrTail = msg;
    }
    pthread_mutex_unlock(&this->m_pLock);
}
#endif /* __MSG_QUEUE_H__ */




