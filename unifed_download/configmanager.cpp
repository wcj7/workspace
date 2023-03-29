#include "configmanager.h"

#include "utils.h"

#include <cstdlib>
#include <cstdio>

#include <iostream>

using namespace std;

ConfigManager *ConfigManager::sMgr = NULL;

ConfigManager::ConfigManager(): iniFile()
{
    
}

ConfigManager &ConfigManager::instance()
{
    if (NULL == sMgr) {
        sMgr = new ConfigManager();
    }
    return (*sMgr);
}

bool ConfigManager::init(string fileName)
{
    if (!iniFile.load_ini(fileName)) {
        cout << "load " << fileName << " error" << endl;
        return false;
    }
    
    //ftp config
    ftpUsername = iniFile.read_string("ftp", "username", "root");
    ftpPassword = iniFile.read_string("ftp", "password", "");
    ftpIp = iniFile.read_string("ftp", "ip", "");
    ftpPath = iniFile.read_string("ftp", "path", "/");
    if ('/' != ftpPath.at(ftpPath.length() - 1)) {
        ftpPath = ftpPath + "/";
    }
    
    downloadPath = iniFile.read_string("common", "download_path", "");
    if ('/' != downloadPath.at(downloadPath.length() - 1)) {
        downloadPath = downloadPath + "/";
    }
    
    playlistFileName = iniFile.read_string("common", "playlist_filename", "playlist.xml");
    playlistTempName = iniFile.read_string("common", "playlist_tempname", "playlist_new.xml");
    playlistConvertName = iniFile.read_string("common", "playlist_convertname", "playlist.ini");
    
    finalSuccessCommand = iniFile.read_string("common", "final_success_command", "");
    finalFailedCommand = iniFile.read_string("common", "final_failed_command", "");
    checkInterval = iniFile.read_int("common", "check_interval", 300);
    
    timeFormat = iniFile.read_string("common", "time_format", "%Y/%m/%d %H:%M:%S");
    
    isMaster = (1 == iniFile.read_int("heart_beat", "is_master", 0)) ? true : false;
    heartIp = iniFile.read_string("heart_beat", "heart_ip", "");
    heartPort = iniFile.read_int("heart_beat", "heart_port", 0);
    
    //read log config
    logDir = iniFile.read_string("log", "log_dir", "/var/log/media_update/");
    logFlag = (1 == iniFile.read_int("log", "log_flag", 0)) ? true : false;
    logKeepdays = iniFile.read_int("log", "log_keepdays", 10);
    
    outputStr << "\t\tftpIp: " << ftpIp << ", ftpPath: " << ftpPath << endl;
    outputStr << "\t\tftpUsername: " << ftpUsername << ", ftpPassword: " << ftpPassword << endl;
    outputStr << "\t\tdownloadPath: " << downloadPath << ", playlistFileName: " << playlistFileName << endl;
    outputStr << "\t\tplaylistTempName: " << playlistTempName << ", playlistConvertName: " << playlistConvertName << endl;
    outputStr << "\t\tcheckInterval: " << checkInterval << endl;
    outputStr << "\t\tfinalSuccessCommand: " << finalSuccessCommand << endl;
    outputStr << "\t\tfinalFailedCommand: " << finalFailedCommand << endl;
    outputStr << "\t\tisMaster: " << isMaster << ", heartIp: " << heartIp << ", heartPort: " << heartPort << endl;
    outputStr << "\t\tlog_dir: " << logDir << ", log_flag: " << logFlag << ", keepdays: " << logKeepdays << endl;
    
    return true;
}
