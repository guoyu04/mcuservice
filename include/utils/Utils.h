/**
 * Utils.h
 * 共通工具类
 * Created by guoyu on 2019/8/27.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Utils
{
public:
    static std::string intToString(int value);
    static void thrsleep(int ms);//延时(毫秒)
    static short reverse16(short sVal);            // short型变量的大小端转换
    static int   reverse32(int iVal);              // int型变量的大小端转换
    // 计算异或校验码
    static char calXorValCheckCode(char *pchData, int iDataLen);
    static short calAndCheckCode(char *pchData, int iDataLen);
    static int getCallingPid();
    static int getCallingUid();
    static int getFileLen(const char *path);
    static void hexEncodeFmt(const char *digest,char *str,int length);
    static void hexEncode(char *digest,char *str,int length);
    static void hexDecode(char *str,char *digest,int digestLen);
    static double long2Double(long a);
};

#endif
