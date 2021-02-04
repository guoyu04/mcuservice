/**
 * Common.h
 * 公用常量及结构定义头文件 
 * Created by guoyu on 2019/8/27.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <map>
#include <vector>
#include <iostream>
#include <fcntl.h>
#include <termios.h>


/*-------------------------------常量定义------------------------------------*/

#define RTN_SUCCESS                 (0)         // 成功 正常
#define RTN_FAIL                    (-1)        // 失败 异常

#define ABS(val)   (((val) > 0) ? (val) : (-(val)))    // 计算绝对值

#define PATH_LEN_MAX                (128)       // 路径长度最大值

// 模块类型定义
#define DEV_CMD_TYPE_INVALID        (0x00)      // 无效的模块类型
#define DEV_CMD_TYPE_MIN            (0x02)      // 模块类型最小值(校验使用)
#define DEV_CMD_TYPE_MAX            (0x10)      // 模块类型最大值(校验使用)

//状态查询
#define DEV_CMD_STATUS_QUERY        (0x81)      //发送状态查询cmd
#define DEV_CMD_STATUS_REC          (0x01)      //MCU状态回复

#define DEV_POWER_ON                (1)         //电源开
#define DEV_POWER_OFF               (0)         //电源关闭


//音源输入通道
#define AUDIO_CHANNEL_FM             0          //FM输入   
#define AUDIO_CHANNEL_MEDIA          1          //media输入  


#define FD_INVALID                  (-1)        // Fd的无效值

#define DEV_OPEN_SUCCESS            RTN_SUCCESS // 打开状态 OK
#define DEV_OPEN_FAIL               RTN_FAIL    // 打开状态 断开
#define DEV_OPEN_MCU_ERROR          (0x02)      // 打开状态 MCU返回异常
#define DEV_OPEN_MCU_NO_SUP         (0x03)      // 打开状态 MCU返回不支持


#define PACK_LEN_MAX                (50)       // 包的最大长度
#define PACK_LEN_LEN                (2)        // 包长度的长度
#define PACK_CHECKCODE_LEN          (2)        // 校验码的长度
#define PACK_PREFIX_LEN_MAX         (4)        // 包的前缀最大长度
#define PACK_BODY_MIN_MIN           (13)       // 消息体最小长度 8+1+4
#define MSG_HEAD_LEN                (2)        // 消息体命令和子命令长度
#define CAN_ID_LEN                  (4)        // CAN ID长度
#define CAN_DATA_LEN                (8)        // CAN数据内容的长度
#define CAN_LEN_LEN                 (1)        // CAN长度的长度
#define CAN_SPEAK_MAX_LEN           (6)        // CAN语音的最大长度

#define WIFI_HEAD                   "helloworld"     //wifi 头部信息 

#define KEY_DOWN                    0x00       // key按下
#define KEY_UP                      0x01       // key松开
#define KEY_1                       0x01       // key1
#define KEY_2                       0x02       // key2
#define KEY_3                       0x03       // key3
//大小端转换
//short转大端char
#define shortToBigChar2( l, b ) \
    do { \
        *(b) = (l) & 0xff;      \
        *(b+1) = ( (l)>>8 ) & 0xff;     \
}while(0)

//大端char转short
#define bigChar2ToShort(b, l) \
    do {\
        l = ((*(b)) & 0xff) + ( *(b+1) & 0xff) * 256; \
}while(0)

//int转四字节大端char
#define intToBigChar2( l, b ) \
    do { \
        *(b) = (l) & 0xff;      \
        *(b+1) = ( (l)>>8 ) & 0xff;     \
        *(b+2) = ( (l)>>16 ) & 0xff;     \
        *(b+3) = ( (l)>>24 ) & 0xff;     \
}while(0)

//4字节大端char转int
#define bigChar2ToInt(b, l) \
    do {\
        l = ((*(b))&0xff)+((*(b+1)&0xff)<<8)+((*(b+2)&0xff)<<16)+((*(b+3)&0xff)<<24); \
}while(0)

//8字节大端char转int
#define bigChar2ToLong(b, l) \
    do {\
        l = ((*(b))&0xff)+((*(b+1)&0xff)<<8)+((*(b+2)&0xff)<<16)+((*(b+3)&0xff)<<24) \
            +((long)(*(b+4)&0xff)<<32)+((long)(*(b+5)&0xff)<<40)+((long)(*(b+6)&0xff)<<48)+((long)(*(b+7)&0xff)<<56); \
}while(0)

//2字节char转short
#define char2ToShort(b, l) \
    do {\
        l=((*(b+1))&0xff)+(*(b)&0xff)*256; \
}while(0)

//4字节char转int
#define char2ToInt(b, l) \
    do {\
        l=((*(b+3))&0xff)+((*(b+2)&0xff)<<8)+((*(b+1)&0xff)<<16)+((*(b)&0xff)<<24); \
}while(0)
/*-------------------------------类型定义----------------------------------*/

typedef pthread_t          ThreadId;
typedef fd_set             FdSet;
typedef pthread_mutex_t    ThreadMutex;
typedef struct timeval     TTimeVal;

/**
 * acc状态
 */
enum ACC_STATE{
    ACC_STATE_OFF = 0x01,
    ACC_STATE_ACC,
    ACC_STATE_START
};

/**
 * 电源状态
 */
enum POWER_STATE{
    POWER_STATE_NOR = 0x01,
    POWER_STATE_FAIL,
};

/**
 * 档位状态
 */
enum CAR_GEAR{
    CAR_GEAR_P = 0x01,
    CAR_GEAR_R,
    CAR_GEAR_N,
    CAR_GEAR_D,
};
/*-------------------------------结构定义----------------------------------*/
#pragma pack(1)


// 时间数据
typedef struct tagTTimeData
{
    char chYear;   // 年
    char chMonth;  // 月
    char chDay;    // 日
    char chHour;   // 时
    char chMin;    // 分
    char chSec;    // 秒
} TTimeData, *PTTimeData;

//内存对齐，默认8字节
#pragma pack()

class Common
{
public:
    static bool  checkDevCmdType(int iCmdType);    // 检查设备指令类型的有效性  
    static int   getCurTime(PTTimeData ptCurTime);            // 获取当前时间(格式NNYYDDHHMMSS)
};

#endif /* __COMMON_H__ */




