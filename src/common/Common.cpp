/**
 * Common.cpp
 *
 * 实现一些公用的功能 
 * Created by guoyu on 2019/8/27.
 */

#include "common/Common.h"

/**
 * 检查设备指令类型的有效性
 * @param [IN] iCmdType 指令类型值
 * @return true 模块类型有效
 *         false 模块类型无效
 */
bool Common::checkDevCmdType(int iCmdType)
{
    if(((iCmdType < DEV_CMD_TYPE_MIN) && (iCmdType != DEV_CMD_TYPE_INVALID)) || (iCmdType > DEV_CMD_TYPE_MAX))
    {
        return false;
    }

    return true;
}

/**
 * 获取当前时间(格式NNYYDDHHMMSS)
 * @param [OUT] pCurTime 压缩格式的时间
 * @return RTN_SUCCESS 成功 正常
 *         RTN_FAIL 失败 异常
 */
int Common::getCurTime(PTTimeData ptCurTime)
{
    time_t    iCurTime;
    struct tm *ptTime;

    // 获取当前时间
    time(&iCurTime);
    ptTime = localtime(&iCurTime);

    ptCurTime->chYear  = ptTime->tm_year - 100;
    ptCurTime->chMonth = ptTime->tm_mon + 1;
    ptCurTime->chDay   = ptTime->tm_mday;
    ptCurTime->chHour  = ptTime->tm_hour;
    ptCurTime->chMin   = ptTime->tm_min;
    ptCurTime->chSec   = ptTime->tm_sec;

    return RTN_SUCCESS;
}
