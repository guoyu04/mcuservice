/**
 * DecouplerData.cpp
 * 初次分包后的数据
 * Created by guoyu on 2019/9/17.
 */
#include <stdio.h>
#include <string.h>
#include "decoupler/DecouplerData.h"
#include "utils/ZLog.h"


#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.decoupler.DecouplerData"


DecouplerData::DecouplerData(const char* pchData, short sLen)
{
    m_pchData = new char[sLen];
    memcpy(m_pchData, pchData, sLen);
    m_sLen = sLen;
}

DecouplerData::~DecouplerData()
{
    if(m_pchData != NULL)
    {
        delete []m_pchData;
        m_pchData = NULL;
        m_sLen = 0;
    }
}

void DecouplerData::getData(char ** data, short *len)
{
    *data = m_pchData;
    *len = m_sLen;
}


