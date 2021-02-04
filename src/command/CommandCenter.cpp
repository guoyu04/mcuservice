/**
 * CommandCenter.cpp
 * 指令的执行
 * Created by guoyu on 2019/8/27.
 */
#include "command/CommandCenter.h"
#include "utils/ZLog.h"

#include <string>
using std::string;

#ifdef	LOG_TAG
#undef  LOG_TAG
#endif

#define LOG_TAG "mcu.command.commandcenter"


CommandCenter* CommandCenter::m_pCmdCenter = NULL;
ZMutex CommandCenter::s_Lock;

CommandCenter::CommandCenter()
{
    /*for(int i = 0; i < MAX_CMD_ID; i ++)
    {
        m_pCmdArray[i] = NULL;
    }*/
}

CommandCenter::~CommandCenter()
{
    /*for(int i = 0; i < MAX_CMD_ID; i ++)
    {
        if(m_pCmdArray[i] != NULL)
        {
            delete m_pCmdArray[i];
        }
    }*/
}

CommandCenter* CommandCenter::getInstance()
{
    if( NULL == m_pCmdCenter )
	{
	    ZMutex::Autolock lock(s_Lock);

		if( NULL == m_pCmdCenter ) 
		{
			 m_pCmdCenter = new CommandCenter();
		}

	}

	return m_pCmdCenter;
}
