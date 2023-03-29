#ifndef MANAGER_H
#define MANAGER_H

#include "heartbeatmgr.h"
#include "configmanager.h"
#include "mediaupdate.h"
#include "log.h"

#include <iostream>
#include <string>

using namespace std;

WorkStatus getManagerWorkStatus();
void setManagerWorkMode(bool isWork);

class Manager
{
public:
    ~Manager();
    static Manager &instance();
    
    bool init();
    bool start();
    bool stop();
    
    void setWorkStatus(bool available, bool running) {workStatus.isAvailable = available; workStatus.isRunning = running;}
    WorkStatus getWorkStatus() {return workStatus;}
    void setWorkMode(bool workMode) {this->workMode = workMode;}
    bool getWorkMode() {return this->workMode;}
    
private:
    Manager(ConfigManager &cfg);
    
private:
    static Manager *sMgr;
    
    WorkStatus workStatus;
    bool workMode;
    
    ConfigManager &config;
    
    MediaUpdate mMediaUpdater;
};

#endif // MANAGER_H
