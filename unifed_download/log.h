#ifndef LOG_H
#define LOG_H

#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;

#define MAX_PATH_LEN (256)

class Log
{
public:
    ~Log();
    static Log &instance();
    bool init(string dir, bool flag, int keepDays);
    
    void print_current_time(FILE *fpt);
    bool writeLog2File(const char *format, ...);
    bool removeLogs();
    
private:
    Log();
    
    bool initFp();
    
private:
    static Log *sMgr;
    
    string logDir;
    bool logFlag;
    int logKeepDays;
    FILE *fp;
};

#define LOG_DEBUG(format, ...) Log::instance().writeLog2File(format, ##__VA_ARGS__)

#endif // LOG_H
