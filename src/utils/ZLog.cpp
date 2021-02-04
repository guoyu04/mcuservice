/**
 * ZLog.cpp
 * log工具类
 * Created by guoyu on 2019/8/27.
 */
#include <stdio.h>
#include "utils/ZLog.h"
#include "utils/Utils.h"

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.utils.ZLog"

enum android_LogPriority _log_level = ANDROID_LOG_DEBUG;
static char debug[4096] = {0};
//enum android_LogPriority _log_level = ANDROID_LOG_INFO;
//

/**
 * 打印缓冲区
 * @param [IN] pchBuffName----需要打印缓冲区名字
 * @param [IN] pchBuff--------需要打印缓冲区地址
 * @param [IN] iLen-----------需要打印缓冲区长度
 * @return
 */
void DebugBuffer(const char *TAG, const char * pchLog, const char *pchBuff, int iLen)
{
#if ERROR_PRINT_ON
    memset(debug, 0, sizeof(debug));
    Utils::hexEncodeFmt(pchBuff, debug, iLen);
    LOGD(TAG, "---------start-----------------");
    LOGD(TAG, "%s len = %d\n%s.", pchLog, iLen, debug); 
    LOGD(TAG, "---------end-----------------");
#endif
}

