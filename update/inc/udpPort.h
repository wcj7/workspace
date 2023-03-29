#ifndef __UDP__PORT__H__
#define __UDP__PORT__H__
#include "port.h"
#include "configs.h"

using namespace std;
class udpPort:public portX
{
    int mSockfd;
    bool bValid;
    configs& mConfig;

    public:
    udpPort(configs& c):mConfig(c){bValid =false;}
    bool  initiate(string info = "", u32 data = 0);
    string getCMD();
    void sendMsg(string msg);
    void reset();
    
   // superTask* parse(string str);
};
#endif
