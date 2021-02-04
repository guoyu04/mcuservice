/**
 * GetPkgInfo.h
 * 包信息工具类
 * Created by guoyu on 2019/8/27.
 */
#ifndef _GET_PKG_INFO_H_
#define _GET_PKG_INFO_H_

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

using namespace std;

class GetPkgInfo
{
public:
    static string getPkgPathFromUid( int uid );
    static string getPkgNameFromUid( int uid );
    static string getPkgNameFromPid( int pid );
    static uint32_t getUidFromPkgName( string PkgName );
private:
    GetPkgInfo();
    static string findPkgPathFromXml( string uid );
    static string findPkgNameFromXml( string uid );
    static map< string, string > pkgPathMap;
    static map< string, string > pkgNameMap;
};

#endif
