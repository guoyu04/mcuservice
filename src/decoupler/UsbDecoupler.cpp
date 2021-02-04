/**
 * UsbDecoupler.h
 * USB串口节点分包类 
 * Created by guoyu on 2019/8/27.
 */

#include "decoupler/UsbDecoupler.h"
#include "common/CrcCalc.h"
#include "utils/ZLog.h"
#include "utils/Utils.h"

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.common.UsbDecoupler"

#define PACKET_LEN_LEN 1 //包长度字段的字节数
#define PACKET_REC_PERFIX 0xEE //特殊字符占位符

UsbDecoupler::UsbDecoupler()
{
}

UsbDecoupler::UsbDecoupler(char *pchPrefix, int iPrefixLen, int iQueBufSize) 
                : Decoupler(pchPrefix, iPrefixLen, iQueBufSize)
{
}

UsbDecoupler::~UsbDecoupler()
{
}

/**
 * 从数据中查找包的长度
 * @param [OUT] iPackLen 包内容的长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */
/*int UsbDecoupler::findPacketLength(short &iPackLen)
{
    int iRet = RTN_SUCCESS;
    char sLen;
    iRet = m_ptrCirQue->getData(&sLen, PACKET_LEN_LEN);
    if(RTN_SUCCESS == iRet)
    {
        iPackLen = sLen; 
        LOGE(LOG_TAG,"find packet length [0x%02x]", iPackLen);
    }
    return iRet;
}*/

/**
 * 获取包内容的实际长度
 * 根据查找到的包长度字段，计算出包内容的长度
 * @return 包内容的长度
 */
/*short UsbDecoupler::getPacketContentLen()
{
    return m_sPackLen - PACKET_LEN_LEN;
}*/

/**
 * 获取包内容信息
 * 查找缓冲区数据，是否包含特殊字符替换字符，如果存在，则还原字符
 * @param [OUT] pchContentBuff 包内容缓冲区buffer
 * @param [OUT] iContentLen 包内容长度
 */
/*int UsbDecoupler::getPackContent(char *pchContentBuff,  short &iContentLen)
{
    int iRet = RTN_SUCCESS;
    char chTemp;
    short tmpLen = 0;
    bool bFind;
    short index = 0;
    while(index < iContentLen)
    {
        if((index+m_iPrefixLen) > iContentLen)//假如后面不够获取1个分包器的数据，则终止
        {
            break;
        }
        iRet = m_ptrCirQue->getDataTemp(&chTemp, sizeof(chTemp));
        if(RTN_FAIL == iRet)
        {
            break;
        }

        pchContentBuff[index] = chTemp;
        if(chTemp == PACKET_REC_PERFIX)    // 找到前缀替换字符,则查找后面一个字节
        {
            iRet = m_ptrCirQue->getDataTemp(&chTemp, sizeof(chTemp));
            if(RTN_FAIL == iRet)
            {
                break;
            }
            if(chTemp == 0x01)
            {
                pchContentBuff[index] = 0xff;
            }
            else if(chTemp == 0x02)//如果是EE，则丢弃
            {

            }
            else//不是特殊字符
            {
                pchContentBuff[++index] = chTemp;
            }
        }
        index ++;

    }
    return iRet;
}*/

/**
 * 获取包消息体的实际长度:除去分隔符、包长度标识、校验码后的长度
 */
short UsbDecoupler::getPackageBodyLen()
{
    return getPacketContentLen() - PACK_CHECKCODE_LEN;
}
/**
 * 校验包，计算校验，计算除了起始、结束和校验本身剩下内容
 * @param [IN] pchContentBuff 包数据
 * @param [IN] iContentLen 包长度
 * @return RTN_SUCCESS 成功
 *         RTN_FAIL 失败
 */

int UsbDecoupler::verifyPacketData(char* pchContentBuff, short iPackLen)
{
    int iRet = RTN_SUCCESS;
    //计算校验码
    short checkCode = Utils::calAndCheckCode(pchContentBuff, iPackLen-PACK_CHECKCODE_LEN);
    short dataCode = (pchContentBuff[iPackLen-PACK_CHECKCODE_LEN] & 0xff) + (pchContentBuff[iPackLen-1] & 0xff) * 256;
    iRet = checkCode == dataCode ? RTN_SUCCESS : RTN_FAIL;
    //delete [] pchTemp;
    return iRet;
}
