#ifndef TELNET_H
#define TELNET_H
#include <string>
#include <thread>
#include <functional>
using namespace std;

class telnet
{

    int mSockfd;
    bool bConnected;
    thread mThr; // thread for receive information for telnet
    bool blogin;
    string user;
    string psw;
    function<void(string, int)> mFun;// could be considered as callback 
public:
    telnet();
    telnet(string& u,string p)
    {
        user = u; psw = p;
    }
    ~telnet(){close();}

    /*
    *  connectServer is going to establish TCP connection with ip:23
    *     all information will bring to up layer with callback pf,
    *     return true when it connected, then could invoke "exeCMD" & "close"
    *     and "close()" will be called defaultly in distruction
    */
    bool connectServer(string& ip, function<void(string, int)> pf);
    void exeCMD(string& cmd);
    void close(bool closeSOCK = true);
private:
    void notify(string info, int err);
};

#endif // TELNET_H
