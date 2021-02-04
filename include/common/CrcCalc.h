/**
 * CrcCalc.h
 * crc 运算 
 * Created by guoyu on 2019/8/27.
 */

#ifndef __CRCCALC_H__
#define __CRCCALC_H__

class CrcCalc
{
 public:
    CrcCalc();
    virtual ~CrcCalc();
    virtual unsigned short crc16Calc(unsigned char *pucDt, unsigned short usLen);     // CRC16计算
    virtual unsigned short crc16CalcBig(unsigned char *pucDt, unsigned short usLen);  // CRC16计算(输出为大端模式)
    virtual unsigned char  crc8Calc(unsigned char *pucDt, unsigned short usLen);      // CRC8计算
 private:
    static const unsigned short m_asCrc16Tab[256];  // CRC16表格
    static const unsigned char  m_aucCrc8Tab[256];  // CRC8表格
};

#endif




