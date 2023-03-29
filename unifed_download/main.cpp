#include <iostream>
#include <unistd.h>

#include "configmanager.h"
#include "utils.h"
#include "log.h"
#include "manager.h"

#include <csignal>
#include <string>

using namespace std;

bool runFlag = true;

void signalFun(int sig)
{
    runFlag = false;
}

int main(int argc, char *argv[])
{ 
    cout << "media update, 20220914_1" << endl;
    
    ConfigManager &configMgr = ConfigManager::instance();
    if (!configMgr.init("media_update.ini")) {
        cout << "init config error" << endl;
        return -1;
    }
    
    if (!Log::instance().init(configMgr.logDir, configMgr.logFlag, configMgr.logKeepdays)) {
        cout << "Init log instance error" << endl;
        return -1;
    }
    
    LOG_DEBUG("Application started, runtime:%u\n", Utils::getRunTimeSeconds());
    
    LOG_DEBUG("Config info:\n%s\n", configMgr.outputStr.str().c_str());
    
    if (!Manager::instance().init()) {
        return -1;
    }
    
    if (!Manager::instance().start()) {
        return -1;
    }

    while (runFlag) {
        sleep(2);
    }

    if (!Manager::instance().stop()) {
        return -1;
    }
    
    return 0;
}

