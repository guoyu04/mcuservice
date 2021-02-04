/**
 * Decoupler.h
 * 包分离类 
 * Created by guoyu on 2019/8/27.
 */
#ifndef __DECOUPLER_H__
#define __DECOUPLER_H__
#include "common/CircularQueue.h"
#include "common/Common.h"

#define PREFIX_LEN_MAX               (4)        // 前缀最大长度
//#define PACK_OFFSET_COMMON           (6)        // (前缀+包长度)的字节数
// 包数据偏移
//#define PACK_DATA_OFFSET             (PACK_OFFSET_COMMON + sizeof(TPackInfo))
//#define CRC16_VAL_LEN                (0x02)     // CRC16值长度

// 分包器状态定义
#define PACK_STATUS_INIT             (0x01)     // 初始状态
#define PACK_STATUS_GET_PREFIX       (0x02)     // 找到前缀
#define PACK_STATUS_GET_CONTENT      (0x03)     // 取到长度
#define PACK_STATUS_CHECK_PACK       (0x04)     // 校验包内容
#define PACK_STATUS_DECOUPLE_OK      (0x05)     // 分包完成

class Decoupler
{
public:
    Decoupler();
    Decoupler(char *pchPrefix, int iPrefixLen, int iQueBufSize);
    virtual ~Decoupler();
    virtual int putData(const char *pchBuff, int iCount);         // 往分包器中输入需要分包的数据
    virtual int setPrefix(char *pchPrefix, int iPrefixLen);       // 设置分包器前缀
    virtual bool isDecouplerEmpty();  // 判断分包器中是否还有数据
    virtual short getPacketContentLen();//获取包内容的实际长度:除去分隔符、包长度标识后的长度
    virtual short getPackageBodyLen();
    virtual int getPackInfo(char *pchPackData, int iPackBuffSize, int &iPackLen);  // 获取包描述信息
protected:
    virtual int findPacketPrefix();                                     // 从数据中查找前缀
    virtual int findPacketLength(short &iPackLen);                  // 从数据中查找包的长度
    virtual int getData(char *pchBuff, int iLen, int &iPackLen);  // 获取分包完成的数据
    virtual int getPackContent(char *pchContentBuff,  short &iContentLen);  // 获取包内容信息
    virtual int verifyPacketData(char* pchContentBuff, short iPackLen);//校验包，默认CRC16校验
protected:
    int     m_iStatus;     // 分包器状态
    char    m_achPrefix[PREFIX_LEN_MAX];  // 包前缀
    int     m_iPrefixLen;  // 前缀长度
    short   m_sPackLen;    // 包长度
    CircularQueue *m_ptrCirQue;   // 循环队列
    int     m_iCirQueLen;  // 循环队列缓存长度
    ThreadMutex m_tMutex;  // 放入数据和取数据互斥使用
};


#endif /* __DECOUPLER_H__ */
