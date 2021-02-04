/**
 * Utils.cpp
 * 共通工具类
 * Created by guoyu on 2019/8/27.
 */
#include <stdio.h>
#include <string.h>
#include <binder/IPCThreadState.h>
#include "utils/Utils.h"
#include "utils/ZLog.h"
#include "common/Common.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "Utils"
std::string Utils::intToString(int value)
{
    char temp[255]={'0'};
    sprintf(temp, "%d", value);
    return string(temp);
}

/**
 * 延时(毫秒) 
 **/
void Utils::thrsleep(int ms)
{
	if(ms < 0) select(0, NULL, NULL, NULL, NULL);
	else
	{
		struct timeval tv;
		tv.tv_sec = ms/1000;
		tv.tv_usec = (ms%1000)*1000;
		select(0, NULL, NULL, NULL, &tv);
	}
}

/**
 * short型变量的大小端转换
 * @param [IN] sVal 值
 */
short Utils::reverse16(short sVal)
{
    short sRtn;

    sRtn = (((sVal >> 8) & 0x00FF) | ((sVal << 8) & 0xFF00));

    return sRtn;
}

/**
 * int型变量的大小端转换
 * @param [IN] sVal 值
 */
int Utils::reverse32(int iVal)
{
    int iRtn;

    iRtn = (((iVal >> 24) & 0x000000FF)
          | ((iVal >> 8)  & 0x0000FF00)
          | ((iVal << 8)  & 0x00FF0000)
          | ((iVal << 24) & 0xFF000000));

    return iRtn;
}


/**
 * 计算异或校验码
 * @param [IN] pchData 输入数据
 * @param [IN] iDataLen 输入数据长度
 * @return 校验码 
 */
char Utils::calXorValCheckCode(char *pchData, int iDataLen)
{
    // 后续要添加iDataLen要大于2的校验,暂时未添加
    char chCheckCode = pchData[0] ^ pchData[1];

    for(int iLoop = 2; iLoop < iDataLen; iLoop++)
    {
        chCheckCode ^= pchData[iLoop];
    }

    return chCheckCode;
}

short Utils::calAndCheckCode(char *pchData, int iDataLen)
{
    short chCheckCode = 0;
    for(int iLoop = 0; iLoop < iDataLen; iLoop++)
    {
        chCheckCode += pchData[iLoop] ;
    }
    return chCheckCode;
}
/**
 * 获取远程服务的pid
 */
int Utils::getCallingPid() {
    return android::IPCThreadState::self()->getCallingPid();
}

/**
 * 获取远程服务的uid
 *
 */
int Utils::getCallingUid() {
    return android::IPCThreadState::self()->getCallingUid();
}

/**
 * 获取文件长度
 * @param fp 文件句柄
 * @return 文件长度
 */
int Utils::getFileLen(const char *path)
{
	int fileLen = 0;
    FILE *fp = fopen(path, "rb");
	if (NULL == fp)
	{
        return fileLen;
    }
	fseek(fp, 0, SEEK_END);
	fileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
    return fileLen;
}

/**
*十六进制转字符串
*@param digestdigest 十六进制缓冲区
*@param str 字符串缓冲区
*@param 十六进制缓冲区 数据长度
*/
void Utils::hexEncodeFmt(const char *digest,char *str,int length)
{
	int i,l;
	char tmp[8]={0};
	l=0;
	int tg=0;
	for(i=0;i<length;i++)
	{
		memset(tmp,0x00,8);
		sprintf(tmp,"%02x ",digest[i]);
		tg++;
		if (tg == 16)
		{
			tmp[3]='\n';
			tg=0;
		}
		memcpy(str+l,tmp,strlen(tmp));
		l=strlen((char *)str);
	}
	str[l]=0;
}

/**
*十六进制转字符串
*@param digestdigest 十六进制缓冲区
*@param str 字符串缓冲区
*@param 十六进制缓冲区 数据长度
*/
void Utils::hexEncode(char *digest,char *str,int length)
{
	int i,l;
	char tmp[8]={0};
	l=0;
	int tg=0;
	for(i=0;i<length;i++)
	{
		memset(tmp,0x00,8);
		sprintf(tmp,"%02x",digest[i]);
		//tg++;
		//if (tg == 16)
		//	{
		//	tmp[3]='\n';
		//	tg=0;
		//	}
		memcpy(str+l,tmp,strlen(tmp));
		l=strlen((char *)str);
	}
	str[l]=0;
}

/*
*将进制字符串还原为进制数组
*@param str           16进制字符串，长度要是进制数组的倍
*@param digest        16进制数组
*@param digestLen  16进制数组长度
*/
void Utils::hexDecode(char *str,char *digest,int digestLen)
{
	int i = 0;
	int tmp=0;
	for (i=0;i<digestLen;i++)
	{
		if (str[i*2]>='a')
		{
			tmp=str[i*2]-0x57;
		}
		else
		{
			tmp=str[i*2]-'0';
		}

		tmp*=16;
		if (str[i*2+1]>='a')
		{
			tmp+=str[i*2+1]-0x57;
		}
		else
		{
			tmp+=str[i*2+1]-'0';
		}

		digest[i]=(tmp&0xFF);
	}
	return;
}

static long long int getDoublePower(int e, int m)
{
	long long int s = e;
	for (int i = 1; i<m; i++)
	{
		s *= e;
	}
	return s;
}

double Utils::long2Double(long a)
{
    int s = (a >> 63) & 0xFF;//获得符号位，1表示负数，0表示正数 
	int e = (a >> 52) & 0x7FF;
	e = e - 1023;//获得指数 
	long long int m = a & 0xFFFFFFFFFFFFF | 0x10000000000000;//获得底数 
	//cout<<setiosflags(ios::uppercase)<<hex<<m<<endl; 
	long long int c = 0;
	double v = 0.0, y = 1.0;
	if (e >= 0)//向右移动 
	{
		c = (m >> (52 - e)) & 0xFFFFFFFFFFFFFFFF;//获得整数的二进制 
		long long int b = 0;
		for (int i = 0; i<52 - e; i++)
		{
			b = (b << 1) | 0x01;
		}
		b = b&m;//获得小数的二进制 
		int j = 0;
		for (int i = 52 - e - 1; i >= 0; i--)
		{
			j++;
			y = (double)(((b >> i) & 0x01)*getDoublePower(2, j));
			if (y>0.0)
			{
				v += 1.0 / y;
			}
		}
		v = c + v;
		if (s>0)
		{
			v = -v;
		}
	}
	else//向左移动 
	{
		e = -e;
		c = m;
		int j = 0;
		for (int i = 52 + e - 1; i >= 0; i--)
		{
			j++;
			y = (float)(((c >> i) & 0x01)*getDoublePower(2, j));
			if (y>0.0)
			{
				v += 1.0 / y;
			}
		}
		if (s>0)
		{
			v = -v;
		}
	}
    return v;
}

//获取中文字段的GBK编码值
/*vector<unsigned char> Utils::encode(const string s)
{
    string str = s;
    vector<unsigned char> buff;

    int len = str.length();
    buff.resize(len);
    memcpy(&buff[0], str.c_str(), len);

    return buff;
}*/
