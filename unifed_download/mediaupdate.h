#ifndef MEDIAUPDATE_H
#define MEDIAUPDATE_H

#include "workthread.h"
#include "configmanager.h"

#include <iostream>
#include <string>
#include <vector>

typedef struct UpdateFile_S {
    string fileName;
    string md5sum;
    bool isDownload;
} UpdateFile;


using namespace std;

class MediaUpdate : public WorkThread
{
public:
    MediaUpdate(ConfigManager &cfg);
    ~MediaUpdate();
    
    bool init();
    
protected:
    void workThread();

private:
    bool checkAndUpdate();
    
    bool parsePlaylistFile(string playlist, vector<UpdateFile> &files);
    bool generateNewPlaylistFile(vector<UpdateFile> &files, string convPlaylist);
    
private:
    ConfigManager &config;
    
    static MediaUpdate *sMgr;
    
    bool isUpdate;
};

#endif // MEDIAUPDATE_H
