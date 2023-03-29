#include "log.h"

#include <time.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

Log *Log::sMgr = NULL;

Log::Log() : logDir(""), logFlag(false), logKeepDays(10), fp(NULL)
{

}

bool Log::initFp()
{
    static int day = 0;
    
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    if (NULL == timeinfo) {
        cout << "timeinfo is null" << endl;
        return false;
    }
    
    if (day == timeinfo->tm_mday) {
        //same day
        if (NULL != fp) {
            return true;
        }
    } else {
        if (NULL != fp) {
            fclose(fp);
            fp = NULL;
        }
    }
    day = timeinfo->tm_mday;
    
    stringstream sstr;
    sstr << logDir << (timeinfo->tm_year + 1900) 
         << setw(2) << setfill('0') << (timeinfo->tm_mon + 1) 
         << setw(2) << setfill('0') << timeinfo->tm_mday << ".log";
    cout << "log file: " << sstr.str() << endl;
    fp = fopen(sstr.str().c_str(), "a+");
    if (NULL == fp) {
        perror("open log file error");
        return false;
    }
    
    return true;
}

Log::~Log()
{
    if (NULL != fp) {
        fclose(fp);
        fp = NULL;
    }
}

Log &Log::instance()
{
    if (NULL == sMgr) {
        sMgr = new Log();
    }
    return (*sMgr);
}

bool Log::init(string dir, bool flag, int keepDays)
{
    if ('/' != dir.at(dir.length() - 1)) {
        logDir = dir + "/";
    } else {
        logDir = dir;
    }
    logFlag = flag;
    logKeepDays = keepDays;
    
    string str("mkdir -p ");
    str.append(logDir);
    system(str.c_str());
    
    removeLogs();
    
    return true;
}

void Log::print_current_time(FILE *fpt)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    if (NULL == timeinfo) {
        cout << "timeinfo is null" << endl;
        return;
    }
    
    if (fpt != NULL) {
        fprintf(fpt,"[%04d-%02d-%02dT%02d:%02d:%02d] ",(timeinfo->tm_year + 1900), (timeinfo->tm_mon + 1), timeinfo->tm_mday, 
                timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    } else {
        printf("[%04d-%02d-%02dT%02d:%02d:%02d] ",(timeinfo->tm_year + 1900), (timeinfo->tm_mon + 1), timeinfo->tm_mday, 
               timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    }
    return;
}

//输出日志到文件，每天存储一个文件
bool Log::writeLog2File(const char *format, ...)
{
    if (logFlag) {
        if (!initFp()) {
            return false;
        }
        print_current_time(fp);
        va_list args; 
        va_start(args,format);
        //输出到日志文件
        vfprintf(fp,format, args);
        va_end(args);
        fflush(fp);
    }
    
    print_current_time(NULL);
    va_list args1; 
    va_start(args1,format);
    //输出到标准输出
    vprintf(format, args1);
    va_end(args1);
    
    return true;
}

//删除多余的日志文件，保留最新keeyDays天数的日志
bool Log::removeLogs()
{
    DIR *d = NULL;
    struct  dirent *dp = NULL;
    struct stat st;
    char p[MAX_PATH_LEN] = {0};
    vector<string> vString;
    
    if(stat(logDir.c_str(), &st) < 0 || !S_ISDIR(st.st_mode)) {
        cout << "invalid dir: " << logDir << endl;
        return false;
    }
    
    if (!(d = opendir(logDir.c_str()))) {
        cout << "opendir " << logDir << " error" << endl;
        return false;
    }
    
    while ((dp = readdir(d)) != NULL) {
        if ((!strncmp(dp->d_name, ".", 1)) || (!strncmp(dp->d_name, "..", 2))) {
            continue;
        }
        vString.push_back(dp->d_name);
    }
    closedir(d);
    
    if (vString.size() <= logKeepDays) {
        cout << "no need delete files" << endl;
        return true;
    }
    sort(vString.begin(), vString.end());
    
    for (unsigned int i = 0; i < (vString.size() - logKeepDays); i ++) {
        snprintf(p, sizeof(p) - 1, "%s%s", logDir.c_str(), vString.at(i).c_str());
        cout << "del file: " << p << endl;
        remove(p);
    }
    
    return true;
}
