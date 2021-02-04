/**
 * CircularQueue.cpp
 * 环形队列实现
 * Created by guoyu on 2019/8/27.
 */
#include "common/CircularQueue.h"
#include "common/Common.h"
#include "utils/ZLog.h"

#include <string>
using std::string;

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.common.CircularQueue"


/**
 * 构造函数
 * @param [IN] iBuffSize 队列缓冲区大小
 */
CircularQueue::CircularQueue(int iBuffSize)
{
    m_iBuffSize = iBuffSize;
    m_pchBuff   = new char[iBuffSize];
    m_iFront    = 0;
    m_iRear     = 0;
    m_iTempCnt  = 0;
}


/**
 * 析构函数
 */
CircularQueue::~CircularQueue()
{
    delete[] m_pchBuff;
}


/**
 * 往队列缓冲区中放数据
 * @param [IN] pchBuff 放入数据缓冲区地址
 * @param [IN] iCount 放入数据长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int CircularQueue::putData(const char *pchBuff, int iCount)
{
    if(NULL == pchBuff)
    {
        LOGE(LOG_TAG, "pchBuff is NULL!\n");
        return RTN_FAIL;
    }

    if(iCount > getFreeSpace())
    {
        LOGE(LOG_TAG, "Free space is not enough, need = %d, free = %d\n", iCount, getFreeSpace());
        return RTN_FAIL;
    }
    else
    {
        for(int iLoop = 0; iLoop < iCount; iLoop++)
        {
            m_pchBuff[m_iRear] = pchBuff[iLoop];
            m_iRear++;
            m_iRear %= m_iBuffSize;
        }
    }

    return RTN_SUCCESS;
}


/**
 * 从队列取指定长度的数据
 * @param [OUT] pchBuff 取出数据存放缓冲区地址
 * @param [IN] iCount 取出数据长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int CircularQueue::getData(char *pchBuff, int iCount)
{
    if(NULL == pchBuff)
    {
        LOGE(LOG_TAG, "pchBuff is NULL!\n");
        return RTN_FAIL;
    }

    if(iCount > getUsedSpace())
    {
        LOGE(LOG_TAG,"Get data too long, need = %d, current len = %d", iCount, getUsedSpace());
        return RTN_FAIL;
    }
    else
    {
        for(int iLoop = 0; iLoop < iCount; iLoop++)
        {
            if(isEmpty())
            {
                break;
            }
            else
            {
                pchBuff[iLoop] = m_pchBuff[m_iFront];
                m_iFront++;
                m_iFront %= m_iBuffSize;
            }
        }
    }

    return RTN_SUCCESS;
}

/**
 * 临时从队列缓冲中取数据不移动队头
 * @param [IN] pchBuff 取出数据存放缓冲区地址
 * @param [IN] iCount 取出数据长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int CircularQueue::getDataTemp(char *pchBuff, int iCount)
{
    int iLoop;

    //从上次临时取数据的位置开始
    int iFront = m_iFront + m_iTempCnt;
    iFront %= m_iBuffSize;

    //计算上次临时取数据后剩余数据长度
    int iUsedTemp = ((m_iRear - iFront) + m_iBuffSize) % m_iBuffSize;

    if(NULL == pchBuff)
    {
        LOGE(LOG_TAG,"pchBuff is NULL. ");
        return RTN_FAIL;
    }

    if(iCount > iUsedTemp)
    {
        LOGE(LOG_TAG,"Temp get data too long, need = %d, iUsedTemp len = %d", iCount, iUsedTemp);
        return RTN_FAIL;
    }
    else
    {
        for(iLoop = 0; iLoop < iCount; iLoop++)
        {
            if(iFront == m_iRear)
            {
                break;
            }
            else
            {
                pchBuff[iLoop] = m_pchBuff[iFront];
                iFront++;
                iFront %= m_iBuffSize;
            }
        }

        m_iTempCnt += iLoop;
    }

    return RTN_SUCCESS;
}

/**
 * 取消从队列中临时取数据
 * @return RTN_SUCCESS 成功
 *         RTN_FAI L失败
 */
int CircularQueue::cancelGetDataTemp()
{
    if(0 != m_iTempCnt)
    {
        m_iTempCnt = 0;
    }

    return RTN_SUCCESS;
}

/**
 * 判断队列是否为空
 * @return true 队列为空
 *         false 队列非空
 */
bool CircularQueue::isEmpty()
{
    // 队头等于队尾则为空
    if(m_iFront == m_iRear)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * 判断队列是否已满(队列中有一个元素没有使用，若全部使用，则很难区别队列已满和队列为空的情况)
 * @return true 队列已满
 *         false 队列未满
 */
bool CircularQueue::isFull()
{
    // 队尾的下一个元素为对头则队列已满
    if(m_iFront == ((m_iRear + 1) % m_iBuffSize))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * 获取队列缓冲已使用的空间
 * @return 队列缓冲已使用的空间
 */
int CircularQueue::getUsedSpace()
{
    int iUsedSpace;

    iUsedSpace = ((m_iRear - m_iFront) + m_iBuffSize) % m_iBuffSize;

    return iUsedSpace;
}

/**
 * 获取队列缓冲剩余空间
 * @return 队列缓冲剩余空间
 */
int CircularQueue::getFreeSpace()
{
    int iFreeSpace;

    iFreeSpace = m_iBuffSize - getUsedSpace() - 1;

    return iFreeSpace;
}

/**
 * 设置对头位置
 * @return RTN_SUCCESS 成功 
 *         RTN_FAIL 失败
 */
int CircularQueue::setFront()
{
    m_iFront += m_iTempCnt;
    m_iFront %= m_iBuffSize;

    m_iTempCnt = 0;

    return RTN_SUCCESS;
}


/**
 * 清空队列
 * @return RTN_SUCCESS 成功 
 *         RTN_FAIL 失败
 */
int CircularQueue::clear()
{
    m_iFront   = m_iRear;   // 队头移动到队尾
    m_iTempCnt = 0;

    return RTN_SUCCESS;
}


/**
 * 打印队列内容
 * @return RTN_SUCCESS 成功 
 *         RTN_FAIL 失败
 */
int CircularQueue::printCirQueBuff()
{
    int iLoop;

    LOGD(LOG_TAG,"###################### CirQue content is: #######################\n");
    for(iLoop = 1; iLoop <= getUsedSpace(); iLoop++)
    {
        LOGD(LOG_TAG," %02x ", (m_pchBuff[(m_iFront + iLoop - 1) % m_iBuffSize] & 0xff));

        if(0 == (iLoop % 16))
        {
            LOGD(LOG_TAG,"\n");
        }
    }
    LOGD(LOG_TAG,"\n###################### CirQue content end ######################\n");

    // 打印队列信息
    LOGD(LOG_TAG,"####CirQue front: %d. ####\n", m_iFront);
    LOGD(LOG_TAG,"####CirQue rear: %d. ####\n", m_iRear);
    LOGD(LOG_TAG,"####CirQue temp get count: %d. ####\n", m_iTempCnt);

    return RTN_SUCCESS;
}

/**
 * 获取临时取数据长度
 * @param [out] iTempDataLen 临时取数据长度
 * @return RTN_SUCCESS 成功 
 *         RTN_FAIL 失败
 */
int CircularQueue::getTempDatalen(int &iTempDataLen)
{
    iTempDataLen = m_iTempCnt;

    return RTN_SUCCESS;
}



