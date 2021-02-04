/**
 * ZLog.h
 * log工具类
 * Created by guoyu on 2019/8/27.
 */
#ifndef __H_ZD_LOG_H__
#define __H_ZD_LOG_H__

#include <android/log.h>

#define ERROR_PRINT_ON true

#ifdef __cplusplus
extern "C" {
#endif

/*
*	ANDROID_LOG_DEBUG,
*	ANDROID_LOG_INFO,
*	ANDROID_LOG_WARN,
*	ANDROID_LOG_ERROR,
*/

extern android_LogPriority _log_level; 
void DebugBuffer(const char *TAG, const char * pchLog, const char *pchBuff, int iLen);

#define _log_with_level(log,LOG_TAG, fmt, s...) \
    (ANDROID_LOG_##log < _log_level ? 0 : __android_log_print(ANDROID_LOG_##log, LOG_TAG,"[0324A]line:%d " fmt,__LINE__, ##s))

#define LOGD(LOG_TAG, fmt, s...) _log_with_level(DEBUG, LOG_TAG,fmt, ##s)
#define LOGI(LOG_TAG,fmt, s...)	_log_with_level(INFO, LOG_TAG,fmt, ##s)
#define LOGW(LOG_TAG,fmt, s...)	_log_with_level(WARN, LOG_TAG,fmt, ##s)
#define LOGE(LOG_TAG,fmt, s...)	_log_with_level(ERROR,LOG_TAG, fmt, ##s)

#ifdef __cplusplus
}
#endif

#endif /*__H_XD_LOG_H__*/
