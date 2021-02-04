/**
 * CircularQueue.h
 * 环形队列头文件 
 * Created by guoyu on 2019/8/27.
 */
#ifndef __CIRCULARQUEUE_H__
#define __CIRCULARQUEUE_H__
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

class CircularQueue
{
public:
    CircularQueue(int iBuffSize);
    ~CircularQueue();
    int  putData(const char *pchBuff, int iCount);   // 往队列缓冲中放数据
    int  getData(char *pchBuff, int iCount);         // 从队列中取数据
    int  getDataTemp(char *pchBuff, int iCount);     // 从队列缓冲中临时取数据
    int  cancelGetDataTemp();                        // 取消从队列中临时取数据
    bool isEmpty();          // 判断队列是否为空
    bool isFull();           // 判断队列是否已满
    int  getUsedSpace();     // 获取队列缓冲已使用的空间
    int  getFreeSpace();     // 获取队列缓冲剩余空间
    int  setFront();         // 设置队头指针
    int  clear();            // 清空队列
    int  printCirQueBuff();  // 打印队列内容
    int  getTempDatalen(int &iTempDataLen);  // 获取临时取数据长度

private:
    char *m_pchBuff;     // 循环队列缓冲区
    int  m_iBuffSize;    // 缓冲区大小
    int  m_iFront;       // 循环队列对头
    int  m_iRear;        // 循环队列队尾
    int  m_iTempCnt;     // 临时取数据数量
};

#endif /* __CIRCULARQUEUE_H__ */




