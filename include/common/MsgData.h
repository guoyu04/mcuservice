/**
 * MsgData.h
 * 消息队列默认数据类型 
 * Created by guoyu on 2019/8/27.
 */

#ifndef __MSG_DATA_H__
#define __MSG_DATA_H__

#include <vector>
#include <string>


class MsgData
{
	public:
		MsgData(int what);
		virtual ~MsgData();
        int getWhat();

        //设置/获取第一个参数
        void setArg1(int arg);
        int getArg1();

        //设置/获取第二个参数
        void setArg2(int arg);
        int getArg2();

        //设置/获取第一个string
        void setData1(std::string data);
        std::string getData1();

        //设置/获取第二个string
        void setData2(std::string data);
        std::string getData2();

        //是否包含string 1
        bool haveData1();
    private:
        std::string m_strData1;
        std::string m_strData2;
        int m_iWhat;
        int m_iArg1;
        int m_iArg2;
        bool m_bHvData;
};

#endif//__DECOUPLER_DATA_H__
