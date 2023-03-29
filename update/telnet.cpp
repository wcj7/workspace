#include "telnet.h"
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include "log.h"
telnet::telnet()
{
   bConnected = false;
}

bool telnet::connectServer(string& ip, function<void(string, int)> pf)
{
    blogin = false;
    bConnected = false;
    LOG_DEBUG("INFO: Telnet %s starting ...\n", ip.c_str());
    mFun = pf;
    mSockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in con;
    con.sin_family = AF_INET;
    con.sin_port = htons(23);  // telnet default port:23
    con.sin_addr.s_addr = inet_addr(ip.c_str());
    int ret = connect(mSockfd, (const struct sockaddr*) &con, sizeof(con));
    if(ret == 0)
    {
      bConnected = true;
      thread t([this](){
        int rt;
        char buf[1024];
        while(bConnected)
        {
            memset(buf, 0, 1024);
            this_thread::sleep_for(std::chrono::milliseconds(50));
            rt = recv(mSockfd, buf, 1023, 0);
            notify(buf, rt);
            if(rt > 0)
            {
            }
            else
            {//thread ending
              break;
            }
        }
      });

      t.swap(mThr);
    }
    else
    {
      LOG_DEBUG("Warning: Telnet socket connect failure\n");
    }
    return bConnected;
}

void telnet::exeCMD(string& cmd)
{
    
   string run_cmd = cmd + "\r\n";
   LOG_DEBUG("INFO:telnet::%s CMD:%s\n",__FUNCTION__, run_cmd.c_str());
   if(bConnected)
   {
      send(mSockfd, run_cmd.c_str(), run_cmd.size(), 0);		
   }
   else
   {
      LOG_DEBUG("Error: Telnet socket isn't connectted, CMD sending:%s\n", run_cmd.c_str());
   }
}

void telnet::notify(string info, int err)
{
    if(err <= 0)
    {//socket close or error happens
      LOG_DEBUG("Warning: Telnet socket error %d\n", err);
      mFun(info, -1);
    }
    else
    {
       LOG_DEBUG("Telnet: %s\n", info.c_str());
       if(!blogin)
       {
          if(string::npos != info.find("login"))
          {
            exeCMD(user);//user name
          }
          else if(string::npos != info.find("Password"))
          {
            exeCMD(psw); // input password
            this_thread::sleep_for(std::chrono::milliseconds(50));
            blogin = true;
            mFun(info, 0);//login
          }
       }
       else
       {
         mFun(info, 1); //bring out the information to outside
       }
    }
}

void telnet::close(bool closeSOCK)
{
    if(bConnected)
    {
      shutdown(mSockfd,2);
      bConnected = false;
      mThr.join();
    }
    blogin = false;
}
