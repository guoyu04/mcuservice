/**
 * Decoupler.h
 * 包分离类:维护一个环形数据队列，获取包时从环形队列中根据设置的分隔符解析数据，并进行校验，校验成功，则分离出一包数据
 * Created by guoyu on 2019/8/27.
 */

#include "decoupler/Decoupler.h"
#include "common/CircularQueue.h"
#include "common/CrcCalc.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"


#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.common.Decoupler"

Decoupler::Decoupler()
{
}

/**
 * 构造函数
 */
Decoupler::Decoupler(char *pchPrefix, int iPrefixLen, int iQueBufSize)
{
    int iRet;

    m_iStatus = PACK_STATUS_INIT;

    iRet = setPrefix(pchPrefix, iPrefixLen);
    if(RTN_FAIL == iRet)
    {
        LOGE(LOG_TAG,"Set prefix error");
        exit(-1);
    }

    m_ptrCirQue   = (CircularQueue*) new CircularQueue(iQueBufSize);
    m_iCirQueLen = iQueBufSize;
    pthread_mutex_init(&m_tMutex, NULL);
}

Decoupler::~Decoupler()
{
    delete m_ptrCirQue;
}

/**
 * 往分包器中输入需要分包的数据
 * @param [IN] pchBuff 需要分包的数据
 * @param [IN] iCount 放入数据长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int Decoupler::putData(const char *pchBuff, int iCount)
{
    int iRet;

    if(NULL == pchBuff)
    {
        LOGE(LOG_TAG,"putData error pchBuff is NULL !!! ");
        return RTN_FAIL;
    }

    pthread_mutex_lock(&m_tMutex);

    iRet = m_ptrCirQue->putData(pchBuff, iCount);
    if(RTN_SUCCESS != iRet)
    {
        LOGE(LOG_TAG,"Put data into cirque error");
        //PrintBuffer((char*)("pchBuff"), (char*)pchBuff, iCount);
    }

    pthread_mutex_unlock(&m_tMutex);

    return iRet;
}

/**
 * 从数据中查找包的长度,并把长度字段从环形队列移除
 * @param [OUT] iPackLen 包内容的长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int Decoupler::findPacketLength(short &iPackLen)
{
    int iRet = RTN_SUCCESS;
    char sLen[PACK_LEN_LEN];
    iRet = m_ptrCirQue->getData(sLen, PACK_LEN_LEN);
    if(RTN_SUCCESS == iRet)
    {
        bigChar2ToShort(sLen, iPackLen);
    }
    return iRet;
}

/**
 * 获取包内容的实际长度:除去分隔符、包长度标识后的长度
 * 根据查找到的包长度字段，计算出包内容的长度
 * @return 包内容的长度
 */
short Decoupler::getPacketContentLen()
{
    return m_sPackLen - m_iPrefixLen - PACK_LEN_LEN;
}

/**
 * 获取包消息体的实际长度:除去分隔符、包长度标识、校验码后的长度
 */
short Decoupler::getPackageBodyLen()
{
    return getPacketContentLen();
}

/**
 * 获取包描述信息
 * @param [OUT] pchPackData 接收包数据的缓冲区
 * @param [IN]  iPackBuffSize 接收包数据的缓冲区长度
 * @param [OUT] iPackLen 包长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int Decoupler::getPackInfo(char *pchPackData, int iPackBuffSize, int &iPackLen)
{
    char achBuff[PACK_LEN_MAX] = {0};
    int  iPackSize;
    int  iResult;

    iResult = getData(achBuff, PACK_LEN_MAX, iPackSize);
    if(RTN_SUCCESS != iResult)
    {
        LOGE(LOG_TAG,"getData fail. ");
        return RTN_FAIL;
    }

    iPackLen = iPackSize;
    memcpy(pchPackData, achBuff, iPackLen);
    return RTN_SUCCESS;
}

/**
 * 获取分包完成的数据
 * @param [OUT] pchBuff 分包后的包数据(去掉包前缀和最后两位CRC校验)
 * @param [IN] iLen 数据长度
 * @param [OUT] iPackLen 分包后的包长度(去掉包前缀和最后两位CRC校验长度)
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int Decoupler::getData(char *pchBuff, int iLen, int &iPackLen)
{
    int iRet = RTN_SUCCESS;
    if(NULL == pchBuff)
    {
        LOGE(LOG_TAG,"pchBuff is NULL. ");
        return RTN_FAIL;
    }
    pthread_mutex_lock(&m_tMutex);

    while(1)
    {
        switch(m_iStatus)
        {
            case PACK_STATUS_INIT:
            case PACK_STATUS_DECOUPLE_OK:
            {
                if(RTN_SUCCESS == findPacketPrefix())
                {
                    m_iStatus = PACK_STATUS_GET_PREFIX;
                }
                else
                {
                    LOGE(LOG_TAG,"Find prefix error. ");
                    pthread_mutex_unlock(&m_tMutex);
                    return RTN_FAIL;
                }
            }
            case PACK_STATUS_GET_PREFIX:
            {
                // 取得包长度
                iRet = findPacketLength(m_sPackLen);
                if(RTN_SUCCESS == iRet && m_sPackLen <= m_iCirQueLen)//返回成功，且包长度不非法
                {
                    m_iStatus  = PACK_STATUS_GET_CONTENT;
                }
                else
                {
                    LOGE(LOG_TAG,"Get packet len error: return[%d]-packet len[%d]--cirquelen[%d] ", iRet, m_sPackLen, m_iCirQueLen);
                    m_ptrCirQue->clear();    // 清空队列
                    m_sPackLen = 0;
                    m_iStatus = PACK_STATUS_INIT; // 将分包器设置为初始状态
                    pthread_mutex_unlock(&m_tMutex);
                    return RTN_FAIL;
                }
            }
            case PACK_STATUS_GET_CONTENT:
            {
                short iContentLen = getPacketContentLen();
                if(iContentLen > iLen)  // 包长度大于存放返回数据缓存长度
                {
                    LOGE(LOG_TAG,"Buffer len is not enough, packContentlen = %d, buffer len = %d. ", iContentLen, iLen);
                    m_iStatus = PACK_STATUS_INIT;
                    pthread_mutex_unlock(&m_tMutex);

                    return RTN_FAIL;
                }
                else if(iContentLen > m_ptrCirQue->getUsedSpace()) // 包长度大于剩余数据长度
                {
                    LOGE(LOG_TAG,"Current packet data is not enough, packContentlen = %d, data len = %d. ",  iContentLen, m_ptrCirQue->getUsedSpace());
                    pthread_mutex_unlock(&m_tMutex);
                    return RTN_FAIL;
                }
                else   // 正常分包
                {
                    memcpy(pchBuff, m_achPrefix, m_iPrefixLen);//放入前缀
                    shortToBigChar2(m_sPackLen, pchBuff+m_iPrefixLen);//放入长度

                    iRet = getPackContent(pchBuff+m_iPrefixLen+PACK_LEN_LEN, iContentLen);
                    if(RTN_SUCCESS == iRet)
                    {
                       m_iStatus = PACK_STATUS_CHECK_PACK; 
                    }
                    else
                    {
                        LOGE(LOG_TAG,"Get packet data error length = %d. ", iContentLen);
                        m_ptrCirQue->cancelGetDataTemp();
                        pthread_mutex_unlock(&m_tMutex);
                        return RTN_FAIL;
                    }
                }
            }
            case PACK_STATUS_CHECK_PACK:
            {
                //校验包
                iRet = verifyPacketData(pchBuff, m_sPackLen);
                if(RTN_SUCCESS == iRet) //校验通过
                {
                    iPackLen = m_sPackLen;
                    //LOGD(LOG_TAG,"Decoupler getPackage success %d. ", iPackLen);
                    //DebugBuffer(LOG_TAG, "getPackageInfo",  (char*)pchBuff, m_sPackLen);
                    //memcpy(pchBuff, pchTemp, iPackLen);
                    m_ptrCirQue->setFront();
                    m_iStatus = PACK_STATUS_DECOUPLE_OK;
                    pthread_mutex_unlock(&m_tMutex);
                    return RTN_SUCCESS;
                }
                else
                {
                    LOGE(LOG_TAG,"Decoupler crc16Calc check error. ");
                    m_ptrCirQue->cancelGetDataTemp();
                    m_iStatus = PACK_STATUS_INIT;
                    pthread_mutex_unlock(&m_tMutex);
                    continue;
                }
            }
        }
    }

    return RTN_SUCCESS; // 代码走不到这里，消除警告加了该语句
}

/**
 * 获取包内容信息
 * @param [OUT] pchContentBuff 包内容缓冲区buffer
 * @param [OUT] iContentLen 包内容长度
 */
int Decoupler::getPackContent(char *pchContentBuff,  short &iContentLen)
{
    int iRet = RTN_SUCCESS;
    iRet = m_ptrCirQue->getDataTemp(pchContentBuff, iContentLen);
    //iRet = m_ptrCirQue->getData(pchBuff, iContentLen);
    return iRet;
}
/**
 * 校验包，默认CRC16校验
 * @param [IN] pchContentBuff 包数据
 * @param [IN] iContentLen 包长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int Decoupler::verifyPacketData(char* pchContentBuff, short iPackLen)
{
    int iRet = RTN_SUCCESS;
    /*int  iOffset  = 0;
    char *pchTemp = new char[m_sPackLen];

    short sLen = 0;

    memset(pchTemp, 0x00, m_sPackLen);
    // 将包前缀、包长度以及包数据放入缓冲区以进行CRC校验
    memcpy(pchTemp, m_achPrefix, m_iPrefixLen);//放入前缀
    iOffset += m_iPrefixLen;

    sLen = Utils::reverse16(m_sPackLen); // 大小端转换
    memcpy((char*)(pchTemp + iOffset), (char*)(&sLen), sizeof(sLen));//放入长度
    iOffset += sizeof(sLen);

    memcpy((char*)(pchTemp + iOffset), pchContentBuff, iContentLen);//放入数据*/

    CrcCalc crc16;
    iRet = crc16.crc16Calc((unsigned char*)pchContentBuff, iPackLen);//CRC返回0则校验通过
    //delete [] pchTemp;
    return iRet == 0 ? RTN_SUCCESS : RTN_FAIL;
}

/**
 * 设置分包器包前缀
 * @param [IN] pchPrefix 需要分包的数据
 * @param [IN] iPrefixLen 数据长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int Decoupler::setPrefix(char *pchPrefix, int iPrefixLen)
{
    int iLen = 0;

    if(NULL == pchPrefix)
    {
        LOGE(LOG_TAG,"Pointer pchPrefix is NULL");
        return RTN_FAIL;
    }

    memset(m_achPrefix, 0x00, PREFIX_LEN_MAX);

    if(iPrefixLen > PREFIX_LEN_MAX)
    {
        iLen = PREFIX_LEN_MAX;
    }
    else
    {
        iLen = iPrefixLen;
    }

    m_iPrefixLen = iLen;
    memcpy(m_achPrefix, pchPrefix, iLen);

    return RTN_SUCCESS;
}

/**
 * 从数据中查找前缀
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
int Decoupler::findPacketPrefix()
{
    char chTemp;
    char achTempBuff[PACK_PREFIX_LEN_MAX] = {0};
    int  iRet  = RTN_SUCCESS;
    int  iResult;
    bool bFind = false;

    while(RTN_SUCCESS == iRet)
    {
        iRet = m_ptrCirQue->getDataTemp(&chTemp, sizeof(chTemp));
        if(RTN_FAIL == iRet)
        {
            break;
        }
        else
        {
            if(chTemp == m_achPrefix[0])    // 找到前缀首字符
            {
                if(m_iPrefixLen == 1)//假如长度为1，则找到前缀
                {
                    bFind = true;
                    break;
                }
                iRet = m_ptrCirQue->getDataTemp(achTempBuff, m_iPrefixLen - 1);
                if(RTN_SUCCESS == iRet)
                {
                    if(0 == memcmp(achTempBuff, &m_achPrefix[1], m_iPrefixLen - 1))  // 找到前缀剩余字符
                    {
                        bFind = true;
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                continue;
            }
        }
    }

    if(iRet != RTN_SUCCESS)
    {
        LOGE(LOG_TAG,"Unpack data failed. ");
        m_ptrCirQue->printCirQueBuff();
    }

    iResult = m_ptrCirQue->setFront();   // 设置队头
    if(RTN_SUCCESS != iResult)
    {
        LOGE(LOG_TAG,"Set cirque front fail. ");
    }

    if((RTN_SUCCESS == iRet) && bFind)
    {
        return RTN_SUCCESS;
    }
    else
    {
        LOGE(LOG_TAG,"Find prefix error. ");
        return RTN_FAIL;
    }
}

/**
 * 分包器中是否还有数据
 * @return true 分包器中没有数据
 *         false 分包器中还有数据
 */
bool Decoupler::isDecouplerEmpty()
{
    return m_ptrCirQue->isEmpty();
}




