/**
 * AidMethods.cpp
 * 应用管理类
 * Created by guoyu on 2019/8/27.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "utils/AidMethods.h" 
#include "utils/ZLog.h"

#ifdef  LOG_TAG
#define LOG_TAG
#endif 

#define LOG_TAG "mdmc.utils.AidMethods"

#define LC_LINE_BF 512

static pid_t parse_pid(char *msg)
{
	char *p = NULL;

	strtok(msg, " ");
	if((p = strtok(NULL, " ")) == NULL)
		return 0;
	return (pid_t)atoi(p);
}

int AidMethods::killProcessByApkName(const string& apkname)
{
	FILE *fp = NULL;
	char exe_buf[LC_LINE_BF] = {0};
	char line_buf[LC_LINE_BF] = {0};

	if (apkname.empty())
	{
		LOGD(LOG_TAG, "Apk Name = [%s], is empty.\n", apkname.c_str());
		return 1;
	}
	sprintf(exe_buf, "ps | grep %s | grep -v grep", apkname.c_str());

	if ((fp = popen(exe_buf, "r")) == NULL)
	{
		LOGE(LOG_TAG, "popen error, cmd[%s], errmsg[%s]\n", exe_buf,
				strerror(errno));
		return 2;
	}
	while((fgets(line_buf, LC_LINE_BF-1, fp)) != NULL) 
	{
		LOGD(LOG_TAG, "line_buf = %s", line_buf);
		pid_t pid = parse_pid(line_buf);
		if (pid <= 0)
		{
			memset(line_buf, 0x0, LC_LINE_BF);
			continue;
		}
		if (kill(pid, SIGKILL) == 0)
		{
			LOGI(LOG_TAG, "kill pid[%d] apkname[%s] success.\n", 
					pid, apkname.c_str());
		}
		else
		{
			LOGW(LOG_TAG, "kill pid[%d] apkname [%s] fail, errmsg[%s].\n", 
					pid, apkname.c_str(), strerror(errno));
		}
		memset(line_buf, 0x0, LC_LINE_BF);
	}

	pclose(fp);
	return 0;
}

int AidMethods::killProcessByPid(const int pid)
{
	return kill(pid, SIGKILL);
}

#define LC_FILENAME_LEN 258 
#define LC_CACHE_LOG_SIZE 10000
#define LC_DIR	"/data/system"

int AidMethods::setCrashHandler()
{
	char filename[LC_FILENAME_LEN] = {0};

	pid_t pid = fork();

	if (pid < 0)
	{
		LOGE(LOG_TAG, "fork error, errmsg[%s]\n",
				strerror(errno));
		exit(1);
	}

	if (pid > 0)
	{
		int stus = 0;
		if(pid == wait(&stus))
		{
			sprintf(filename, "%s/pid_%d_%d_dump.log", LC_DIR, pid, stus);
			LOGD(LOG_TAG, "cache log path = [%s], size [%u]\n", filename, LC_CACHE_LOG_SIZE);
			cacheLogcat(filename, LC_CACHE_LOG_SIZE);
		}
		exit(0);
	}

	return 0;
}

int AidMethods::cacheLogcat(const char* path, unsigned int n)
{
	int ret = 0;
	char mbuf[BUFSIZ]	= {0};

	LOGD(LOG_TAG, "enter cache logcat, path[%s], size[%u]\n", path, n);
	FILE* tg_p = fopen(path, "w");
	if (tg_p == NULL)
	{
		ret = errno;
		LOGE(LOG_TAG, "fopen error, path[%s], errmsg[%s]\n", path,
				strerror(errno));
		return ret;
	}
	FILE* lg_p = popen("logcat", "r");
	if (lg_p == NULL)
	{
		ret = errno;
		LOGE(LOG_TAG, "popen error, cmd[logcat], errmsg[%s]\n",
				strerror(errno));
		return ret;
	}

	LOGD(LOG_TAG, "fopen, popen both ok.\n");

	for(unsigned int i=0; i<n; i++)
	{
		LOGD(LOG_TAG, "----------------> line [%d] <---------------\n", i);
		if(fgets(mbuf, BUFSIZ-1, lg_p) == NULL)
		{
			ret = errno;
			LOGE(LOG_TAG, "fgets error, %s\n", strerror(errno));
			break;
		}

		LOGD(LOG_TAG, "read msg [%s]\n", mbuf);
		fputs(mbuf, tg_p);
	}

	LOGD(LOG_TAG, "cache log finished.\n");
	fclose(tg_p);
	pclose(lg_p);
	return ret;
}
