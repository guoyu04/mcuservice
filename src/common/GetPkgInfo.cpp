/**
 * GetPkgInfo.cpp
 * 包信息工具类
 * Created by guoyu on 2019/8/27.
 */
#include "common/GetPkgInfo.h"
#include "utils/ZLog.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "GetPkgInfo"

map< string, string > GetPkgInfo::pkgPathMap;
map< string, string > GetPkgInfo::pkgNameMap;

/**
 * 根据uid获得包的绝对路径
 *
 * @param[in] 要查的包的uid 
 *
 * @return 返回包的绝对路径
 * */
string GetPkgInfo::getPkgPathFromUid( int uid )
{
    string result = "";

    stringstream ss;
    ss << uid;
    string uidStr = ss.str();

    map<string, string>::iterator iter;
    iter = pkgPathMap.find( uidStr );
    if( iter != pkgPathMap.end() )
    {
        result = iter->second;    
        fstream judgeFile;
        judgeFile.open( result.c_str(),ios::in );
        if( !judgeFile )
        {
            result = findPkgPathFromXml( uidStr );
        }
        judgeFile.close();
    }
    else
    {
        result = findPkgPathFromXml( uidStr );
    }
    return result;
}

/**
 * 根据uid获得包名
 *
 * @param[in] 要查的包的uid 
 *
 * @return 返回包名
 * */
string GetPkgInfo::getPkgNameFromUid( int uid )
{
    string result = "";

    stringstream ss;
    ss << uid;
    string uidStr = ss.str();

    map<string, string>::iterator iter;

    iter = pkgNameMap.find( uidStr );
    if( iter != pkgNameMap.end() )
    {
        result = iter->second;    
    }
    else
    {
        result = findPkgNameFromXml( uidStr );
    }
    return result;
}

/**
 * 根据pid获得包名
 *
 * @param[in] 要查的包的pid 
 *
 * @return 返回包名
 * */
string GetPkgInfo::getPkgNameFromPid( int pid )
{
    string result = "";

    stringstream ss;
    ss << pid;
    string pidStr = ss.str();
    
    string cmdlinePath = "";
    cmdlinePath = cmdlinePath + "/proc/" + pidStr + "/cmdline";

    ifstream fs( cmdlinePath.c_str() );
    if( !fs )
    {
        return result;    
    }
    
    string lineStr;
    getline( fs, lineStr );

    result = lineStr.c_str();

    return result;
}

/**
 * (内部函数使用)
 * 从xml文件中获得UID对应路径
 *
 */
string GetPkgInfo::findPkgPathFromXml( string uid )
{
    string result = "";

    ifstream fs("/data/system/packages.xml");
    if( !fs )
    {
        return result;    
    }

    string userId = "userId=\"" + uid + "\"";
    string lineStr;
    string codePath = "codePath=\"";
   
    while( getline( fs, lineStr) )
    {
        string::size_type idx = lineStr.find( userId );
        if( idx != string::npos )
        {
            idx = lineStr.find( codePath );
            
            int start = lineStr.find( "\"" , idx+10 );
            int n = start - idx - 10;

            result = lineStr.substr( idx+10 , n );

            {
                stringstream ss;
                ss << uid;
                string uidStr = ss.str();
                map<string, string>::iterator iter;
                iter = pkgPathMap.find( uidStr );

                if( iter == pkgPathMap.end() )
                {
                    pkgPathMap.insert( pair<string,string>( uid, result ) );
                }
                else
                {
                    iter->second = result; 
                }
            }

            break;
        }
    }
    fs.close();

    return result;
}
/**
 * (内部函数使用)
 * 从xml文件中获得UID对应的包名
 *
 */
string GetPkgInfo::findPkgNameFromXml( string uid )
{
    string result = "";

    ifstream fs("/data/system/packages.list");
    if( !fs )
    {
        return result;    
    }

    string lineStr;
   
    while( getline( fs, lineStr) )
    {
        string::size_type idx = lineStr.find( uid );
        if( idx != string::npos )
        {
            
            result = lineStr.substr( 0 , idx-1 );

            pkgNameMap.insert( pair<string,string>( uid, result ) );

            break;
        }
    }
    fs.close();

    return result;
}

uint32_t GetPkgInfo::getUidFromPkgName( string PkgName )
{
    string result = "";

    ifstream fs("/data/system/packages.list");
    if( !fs )
    {
	LOGD(LOG_TAG, " cws fs =0 pckname = %s,  uid = %d\n", PkgName.c_str(), atoi(result.c_str()));
	return atoi(result.c_str());      
    }

    string lineStr;
   
    while( getline( fs, lineStr) )
    {
        string::size_type idx = lineStr.find( PkgName );
        if( idx != string::npos )
        {
		    
		string::size_type size=lineStr.size();
		std::string pattern = " ";
		int num =0;
		for(string::size_type i=0; i<size; i++)
		{
			string::size_type pos=lineStr.find(pattern,i);
			if(pos<size)
			{
				std::string s=lineStr.substr(i,pos-i);
				i=pos+pattern.size()-1;
				//LOGI(LOG_TAG, " cws s = %s\n", s.c_str());
				num ++;
				result = s;
			}
			if (num ==2)
			{
				break;
			}
		}
            break;
        }
    }
    fs.close();
    LOGD(LOG_TAG, "pckname = %s,  uid = %d\n", PkgName.c_str(), atoi(result.c_str()));

    return atoi(result.c_str());
}
