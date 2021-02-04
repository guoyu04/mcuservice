/**
 * UsbDecoupler.h
 * USB包分离类 
 * Created by guoyu on 2019/8/27.
 */
#ifndef __USB_DECOUPLER_H__
#define __USB_DECOUPLER_H__
#include "decoupler/Decoupler.h"

class UsbDecoupler : public Decoupler
{
public:
    UsbDecoupler();
    UsbDecoupler(char *pchPrefix, int iPrefixLen, int iQueBufSize);
    virtual ~UsbDecoupler();
    virtual short getPackageBodyLen();
    //virtual int findPacketLength(short &iPackLen);                  // 从数据中查找包的长度
    //virtual short getPacketContentLen();//获取包内容的实际长度
    //virtual int getPackContent(char *pchContentBuff,  short &iContentLen);  // 获取包内容信息
protected:
    virtual int verifyPacketData(char* pchContentBuff, short iPackLen);//校验包，亦或校验
};


#endif /* __MCU_DECOUPLER_H__ */
