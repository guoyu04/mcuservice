/**
 * Command.cpp
 * 指令基类
 * Created by guoyu on 2019/8/27.
 */
#include "command/Command.h"
#include "utils/ZLog.h"


#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.command.Command"


Command::Command()
{
}

Command::~Command()
{
}
