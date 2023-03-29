#include "mediaupdate.h"

#include "log.h"
#include "utils.h"
#include "md5.h"
#include "tinyxml.h"
#include "curlutils.h"
#include "manager.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>


MediaUpdate::MediaUpdate(ConfigManager &cfg) : config(cfg)
{
}

MediaUpdate::~MediaUpdate()
{
    
}

bool MediaUpdate::init()
{
    return true;
}

void MediaUpdate::workThread()
{
    uint32_t lastSeconds = Utils::getRunTimeSeconds();
    
    isUpdate = false;
    LOG_DEBUG("Media update thread started...\n");
    
    while (isRun) {
        sleep(10);
        
        LOG_DEBUG("WorkMode: %s\n", (Manager::instance().getWorkMode() ? "true" : "false"));
        if (!Manager::instance().getWorkMode()) {
            continue;
        }
        
        Manager::instance().setWorkStatus(true, true);
        
        uint32_t currSec = Utils::getRunTimeSeconds();
        if (currSec >= (lastSeconds + config.checkInterval)) {
            //check
            checkAndUpdate();
            isUpdate = false;
            lastSeconds = currSec;
        }
    }
}

bool MediaUpdate::checkAndUpdate()
{
    bool ret = false;
    LOG_DEBUG("Start checkAndUpdate...\n");
    if (isUpdate) {
        LOG_DEBUG("Media updating, over\n");
        return true;
    }
    
    isUpdate = true;
    
    string str("mkdir -p ");
    str.append(config.downloadPath);
    system(str.c_str());
    
    stringstream ss;
    ss << "ftp://" << config.ftpUsername << ":" << config.ftpPassword << "@" << config.ftpIp << config.ftpPath;
    string remotePrefix = ss.str();
    
    string remotePlaylist = remotePrefix + config.playlistFileName;
    string localPlaylistFile = config.downloadPath + config.playlistTempName;
    LOG_DEBUG("Start download playlist file: %s \nto %s\n", remotePlaylist.c_str(), localPlaylistFile.c_str());
    
    if (!CurlUtils::downloadFile(remotePlaylist, localPlaylistFile)) {
        return false;
    }
    
    string oldplaylistFile = config.downloadPath + config.playlistFileName;
    string oldmd5sum;
    string md5sum;
    oldmd5sum = FileDigest(oldplaylistFile);
    LOG_DEBUG("%s md5sum: %s\n", config.playlistFileName.c_str(), oldmd5sum.c_str());
    md5sum = FileDigest(localPlaylistFile);
    LOG_DEBUG("%s md5sum: %s\n", config.playlistTempName.c_str(), md5sum.c_str());
    
    if (0 == oldmd5sum.compare(md5sum)) {
        //do not update
        LOG_DEBUG("Md5sum is same, the playlist file is the latest version, do not update!\n");
        return false;
    }
    
    LOG_DEBUG("Md5sum is not same, parse playlist file...\n");
    vector<UpdateFile> updateFiles;
    if (!parsePlaylistFile(localPlaylistFile, updateFiles)) {
        return false;
    }
    
    if (updateFiles.empty()) {
        LOG_DEBUG("There is no update files in playlist file!\n");
        return false;
    }
    LOG_DEBUG("Parse playlist file success, find %d files:\n", updateFiles.size());
    for (vector<UpdateFile>::iterator iter = updateFiles.begin(); iter != updateFiles.end(); iter ++) {
        LOG_DEBUG("Filename: %s, md5um: %s\n", iter->fileName.c_str(), iter->md5sum.c_str());        
    }
    
    int downloadCnt = 0;
    bool isAllSuccess = false;
    int downCycle = 0;
    while (++downCycle <= 5) {
        LOG_DEBUG("Trying time %d, start download video files...\n", downCycle);
        downloadCnt = 0;
        for (vector<UpdateFile>::iterator iter = updateFiles.begin(); iter != updateFiles.end(); iter ++) {
            if (iter->isDownload) {
                //had download
                downloadCnt ++;
                continue;
            }
            //download
            string remoteVideoName = remotePrefix + iter->fileName;
            string localVideoName = config.downloadPath + iter->fileName;
            LOG_DEBUG("Download video file: %s\n", remoteVideoName.c_str());
            if (!CurlUtils::downloadFile(remoteVideoName, localVideoName)) {
                continue;
            }
            
            //cal md5sum
            string fileMd5sum;
            fileMd5sum = FileDigest(localVideoName);
            LOG_DEBUG("Cal md5sum: %s\n", fileMd5sum.c_str());
            if (fileMd5sum.empty()) {
                continue;
            }
            if (0 == fileMd5sum.compare(iter->md5sum)) {
                //download success
                iter->isDownload = true;
                downloadCnt ++;
                LOG_DEBUG("Md5sum is correct, download files: %d\n", downloadCnt);
            }
        }
        LOG_DEBUG("Trying time %d,  all: %d, success: %d\n", downCycle, updateFiles.size(), downloadCnt);
        if (downloadCnt == updateFiles.size()) {
            //all download success
            isAllSuccess = true;
            break;
        }
    }
    
    if (isAllSuccess) {
        //convert playlist file
        string convPlaylist = config.downloadPath + config.playlistConvertName;
        LOG_DEBUG("All files downloaded successfully, generate local playlist file: %s\n", convPlaylist.c_str());
        generateNewPlaylistFile(updateFiles, convPlaylist);
        LOG_DEBUG("Exec final success command: %s\n", config.finalSuccessCommand.c_str());
        system(config.finalSuccessCommand.c_str());
        LOG_DEBUG("Media update success\n");
        ret = true;
    } else {
        //clear all temp files
        LOG_DEBUG("Tried %d times, but the download is still unsuccessful\n", downCycle);
        LOG_DEBUG("Exec final failed command: %s\n", config.finalFailedCommand.c_str());
        system(config.finalFailedCommand.c_str());
        LOG_DEBUG("Media update failed, retry next time...\n");
        ret = false;
    }
    return ret;
}

bool MediaUpdate::parsePlaylistFile(string playlist, vector<UpdateFile> &files)
{
    TiXmlDocument doc(playlist.c_str());
    if (!doc.LoadFile())  {
        LOG_DEBUG("Parse playlist file error!\n");
        return false;
    }
    
    TiXmlElement *root = doc.RootElement();
    TiXmlElement *padlist = root->FirstChildElement("PadList");
    if (NULL == padlist) {
        LOG_DEBUG("There is no PadList in playlist file!\n");
        return false;
    }
    
    const char *countStr = padlist->Attribute("Count");
    if (NULL == countStr) {
        LOG_DEBUG("There is no attribute named Count in PadList Node..\n");
        return false;
    }
    int count = atoi(countStr);
    TiXmlElement *item = NULL;
    char buf[10] = {0};
    files.clear();
    for (int i = 1; i <= count; i ++) {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, 10, "Item%d", i);
        item = padlist->FirstChildElement(buf);
        if (NULL == item) {
            continue;
        }
        UpdateFile file;
        file.fileName = item->Attribute("File");
        string md5sum1 = item->Attribute("Md5sum");
        //convert to lower
        transform(md5sum1.begin(), md5sum1.end(), md5sum1.begin(), ::tolower);
        file.md5sum = md5sum1;
        file.isDownload = false;
        if (file.fileName.empty() || file.md5sum.empty()) {
            LOG_DEBUG("%s: filename or md5sum is empty!\n");
            continue;
        }
        files.push_back(file);
    }
    return true;
}

bool MediaUpdate::generateNewPlaylistFile(vector<UpdateFile> &files, string convPlaylist)
{
    ofstream file(convPlaylist.c_str(), ios::trunc);
    if (!file.is_open()) {
        LOG_DEBUG("Open new playlist file error!\n");
        return false;
    }
    
    //version=2021/03/12 14:55:32
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), config.timeFormat.c_str(), timeinfo);
    
    int i = 0;
    file << "[LIST-0]" << endl;
    file << "version=" << timeStr << endl;
    file << "list_name=occ_update" << endl;
    for (vector<UpdateFile>::iterator iter = files.begin(); iter != files.end(); iter ++) {
        file << "video-" << i << "=/var/ftp/" << iter->fileName << endl;
        i ++;
    }
    return true;
}
