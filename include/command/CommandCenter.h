/**
 * CommandCenter.h
 * 指令的执行
 * Created by guoyu on 2019/8/27.
 */


#ifndef __COMMAND_CENTER_H__
#define __COMMAND_CENTER_H__

#include "utils/ZMutex.h"
#include "command/Command.h"
#include <vector>

using namespace std;

class CommandCenter
{
	private:
		/**
		 * construction and destruction
		 **/
		CommandCenter();
		~CommandCenter();

	public:
		/**
		 * get class instance
		 **/
		static CommandCenter* getInstance();

	private:
		static CommandCenter* m_pCmdCenter;
        static ZMutex s_Lock;
};


#endif
