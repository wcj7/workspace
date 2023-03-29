#pragma once

#include "commonheader.h"
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <string>
using namespace std;

const u32 TCP = 0;
const u32 UDP = 1;

struct netInfo
{
    netInfo(u32 t, function<void(char *, int)> d, function<void(int)> e) : type(t), dataHandler(d), errorHandler(e)
    {
    }
    u32 type; // tcp =0, udp= 1
    function<void(char *, int)> dataHandler;
    function<void(int)> errorHandler;
};

class netService
{
    map<u32, shared_ptr<netInfo>> m_netSet;
    vector<u32> m_ABNSocks;
    void receiveDataHandle(u32);
    void errorHandle(u32);

public:
    // netService();
    u32 createTCPClient(string ip, u16 port);
    u32 createUDPPoint(string ip, u16 port);
    bool setHandler(u32 id, function<void(char *, int)>, function<void(int)>);
    void handle();
    bool sendData(u32 id, u8 *pt, u32 len, string ip = "", u16 port = 0);
    void close(u32);
};

netService *getNetIF();