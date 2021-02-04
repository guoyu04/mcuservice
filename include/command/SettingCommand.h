/**
 * SettingCommand.h
 * 设置指令
 * Created by guoyu on 2019/8/27.
 */

#ifndef __SETTING_CMD_H__
#define __SETTING_CMD_H__
#include "utils/Lock.h"
#include "Command.h"

class SettingCommand : public Command {
	public:
		SettingCommand();
		~SettingCommand();

	private:
		static CLock	m_Lock;
};


#endif
