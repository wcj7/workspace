#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "ini_file.h"
#include "stdint.h"

#include <string>
#include <sstream>
#include <map>

using namespace std;

typedef struct {
    string name;
    string checkDir;
    string command;
    int execTime;
} ExecParam;

class ConfigManager
{
public:
    ConfigManager();
    static ConfigManager &instance();

    bool init(string fileName);
    
public:
    string ftpUsername;
    string ftpPassword;
    string ftpIp;
    string ftpPath;
    
    string downloadPath;
    string playlistFileName;
    string playlistTempName;
    string playlistConvertName;
    string finalSuccessCommand;
    string finalFailedCommand;
    int checkInterval;
    
    string timeFormat;
    
    bool isMaster;
    string heartIp;
    int heartPort;
    
    string logDir;
    bool logFlag;
    int logKeepdays;
    
    //保存待输出的配置信息
    stringstream outputStr;
    
private:
    static ConfigManager *sMgr;
    ini_file iniFile;
    
};

#endif // CONFIGMANAGER_H
