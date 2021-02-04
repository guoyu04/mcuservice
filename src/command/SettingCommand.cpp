/**
 * SettingCommand.cpp
 * 设置指令
 * Created by guoyu on 2019/8/27.
 */
#include <stdio.h>
#include "command/CommandCenter.h"
#include "command/SettingCommand.h"
#include "utils/ZLog.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.command.SettingCommand"

CLock		SettingCommand::m_Lock;

SettingCommand::SettingCommand()
{
	//CommandCenter::getInstance()->addCommand(CMD_DEL_SMS, this);
}

SettingCommand::~SettingCommand()
{
}

