#include "manager.h"

WorkStatus getManagerWorkStatus()
{
    return Manager::instance().getWorkStatus();
}

void setManagerWorkMode(bool isWork)
{
    Manager::instance().setWorkMode(isWork);
}

Manager* Manager::sMgr = NULL;

Manager::Manager(ConfigManager &cfg) : config(cfg), mMediaUpdater(cfg)
{
    workStatus.isAvailable = true;
    workStatus.isRunning = false;
    workMode = false;
}

Manager::~Manager()
{
    
}

bool Manager::init()
{
    if (!HeartbeatMgr::instance().init(getManagerWorkStatus, setManagerWorkMode, config.isMaster, config.heartIp, config.heartPort, 10)) {
        return false;
    }
    
    if (!mMediaUpdater.init()) {
        return false;
    }
    return true;
}

bool Manager::start()
{
    if (!mMediaUpdater.start()) {
        return false;
    }
    return true;
}

bool Manager::stop()
{
    if (!mMediaUpdater.stop()) {
        return false;
    }
    return true;
}

Manager &Manager::instance()
{
    if (NULL == sMgr) {
        sMgr = new Manager(ConfigManager::instance());
    }
    return (*sMgr);
}




