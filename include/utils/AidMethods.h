/**
 * AidMethods.h
 * 应用管理类
 * Created by guoyu on 2019/8/27.
 */

#ifndef __AIDMETHODS_H__
#define __AIDMETHODS_H__
#include <string>

using std::string;

class AidMethods {
	public:
		/*
		 * 0 - ok, other - fail.
		 */
		static int killProcessByApkName(const string& apkname);
		static int killProcessByPid(const int pid);
		static int setCrashHandler();
		static int cacheLogcat(const char *filename, unsigned int n);
	private:

};


#endif
