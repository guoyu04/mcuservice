/**
 * DecouplerData.h
 * 初次分包后的数据 
 * Created by guoyu on 2019/8/27.
 */

#ifndef __DECOUPLER_DATA_H__
#define __DECOUPLER_DATA_H__

class DecouplerData
{
	public:
		DecouplerData(const char* pchData, short sLen);
		virtual ~DecouplerData();
        void getData(char ** data, short *len);
    private:
        char * m_pchData;
        short m_sLen;
};

#endif//__DECOUPLER_DATA_H__
