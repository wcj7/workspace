#include "udpPort.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <memory.h>
#include <iostream>
#include "log.h"
bool  udpPort::initiate(string info, u32 data)
{
    struct sockaddr_in addr;
    mSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&addr, sizeof(addr), 0);
    addr.sin_family = AF_INET;
    if(info == "")
    {
        info = mConfig.getValue("self_ip");
        data = atoi(mConfig.getValue("self_port").c_str());
    }
    addr.sin_port = htons(data);
    addr.sin_addr.s_addr = inet_addr(info.c_str());
    int ret = bind(mSockfd, (struct sockaddr *)&addr, sizeof(addr));
    LOG_DEBUG("INFO:bind IP:%s, port: %d\n", info.c_str(), data);
    bValid = ret ==0?true:false;
    return bValid;
}
string udpPort::getCMD()
{
    if(!bValid)
      return "";

    fd_set fs;
    FD_ZERO(&fs);
    FD_SET(mSockfd, &fs);
    struct timeval  tmInterval;
    tmInterval.tv_sec = 0;
    tmInterval.tv_usec = 0;
    //No waiting
    int r = select(mSockfd+1, &fs, 0, 0, &tmInterval);
    if(r >= 1)
    {
      char buf[600];
      sockaddr_in sender_addr;
      u32 sender_addr_size = sizeof(sender_addr);
      int len = recvfrom(mSockfd, buf, 599, 0, (struct sockaddr *)&sender_addr, &sender_addr_size);
      if(len >=0 && len <599)
      {
          buf[len] = 0;
          LOG_DEBUG("INFO:receive len %d, buffer: %s\n", len, buf);
          string ret(buf);
          return ret;
      }
      else
      {
        LOG_DEBUG("Error: getCMD  %d\n", len);
      }
    }
    return "";
}
void udpPort::sendMsg(string msg)
{
     if(!bValid)
     {
       LOG_DEBUG("Error: udpPort can't send msg for invalid now\n");
       return;
     }
     LOG_DEBUG("INFO: udpPort Send: %s\n", msg.c_str());
     sockaddr_in addr;
     addr.sin_family = AF_INET;
     addr.sin_port = htons(atoi(mConfig.getValue("peer_port").c_str()));
     addr.sin_addr.s_addr = inet_addr(mConfig.getValue("peer_ip").c_str());
     sendto(mSockfd, msg.c_str(), msg.size() , 0, (struct sockaddr *)&addr,  sizeof(addr));
}
void udpPort::reset()
{
    if(bValid)
    {
      shutdown(mSockfd,2);
      bValid = false;
    }

}