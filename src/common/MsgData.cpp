/**
 * MsgData.cpp
 * 消息队列默认数据类型
 * Created by guoyu on 2019/9/17.
 */
#include "common/MsgData.h"
#include "utils/ZLog.h"


#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.common.MsgData"


MsgData::MsgData(int what): m_iWhat(what),m_iArg1(0),m_iArg2(0), m_bHvData(false)
{
}

MsgData::~MsgData()
{
}

int MsgData::getWhat()
{
    return m_iWhat;
}

void MsgData::setArg1(int arg)
{
    m_iArg1 = arg;
}

int MsgData::getArg1()
{
    return m_iArg1;
}

void MsgData::setArg2(int arg)
{
    m_iArg2 = arg;
}

int MsgData::getArg2()
{
    return m_iArg2;
}

void MsgData::setData1(std::string data)
{
    m_strData1 = data;
    m_bHvData = true;
}

std::string MsgData::getData1()
{
    return m_strData1;
}

void MsgData::setData2(std::string data)
{
    m_strData2 = data;
}

std::string MsgData::getData2()
{
    return m_strData2;
}

bool MsgData::haveData1()
{
    return m_bHvData;
}
