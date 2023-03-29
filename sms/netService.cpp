#include "netServer.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "log.h"
#include <string>
#include <string.h>
#include <vector>
// #include <algorithm>
using namespace std;

void unsetDataHandle(char *pb, int len)
{
   LOG_DEBUG("WARNING: Data handler is unsetted, data recv:%d\n", len);
}
void unsetErrorHandle(int err)
{
   LOG_DEBUG("WARNING: Error handler is unsettled, error code %d\n", err);
}
u32 netService::createTCPClient(string ip, u16 port)
{
   u32 sock = socket(AF_INET, SOCK_STREAM, 0);
   struct sockaddr_in con;
   con.sin_family = AF_INET;
   con.sin_port = htons(port); // telnet default port:23
   con.sin_addr.s_addr = inet_addr(ip.c_str());
   int ret = connect(sock, (const struct sockaddr *)&con, sizeof(con));
   if (ret == 0)
   {
      // record info
      m_netSet[sock] = make_shared<netInfo>(0, unsetDataHandle, unsetErrorHandle);
      return sock;
   }
   return 0;
}

u32 netService::createUDPPoint(string ip, u16 port)
{
   struct sockaddr_in addr;
   u32 sock = socket(AF_INET, SOCK_DGRAM, 0);
   memset(&addr, sizeof(addr), 0);
   addr.sin_family = AF_INET;
   addr.sin_port = htons(port);
   addr.sin_addr.s_addr = inet_addr(ip.c_str());
   if (0 == bind(sock, (struct sockaddr *)&addr, sizeof(addr)))
   {
      // recorde socket info
      m_netSet[sock] = make_shared<netInfo>(1, unsetDataHandle, unsetErrorHandle);
      return sock;
   }
   return 0;
}

bool netService::setHandler(u32 id, function<void(char *, int)> d, function<void(int)> e)
{
   if (m_netSet.find(id) == m_netSet.end())
      return false;
   m_netSet[id]->dataHandler = d ? d : unsetDataHandle;
   m_netSet[id]->errorHandler = e ? e : unsetErrorHandle;
   return true;
}

void netService::handle()
{
   fd_set fs, ferr;
   FD_ZERO(&fs);
   FD_ZERO(&ferr);

   if (m_netSet.empty())
      return;
   u32 bigone = 0;
   for (auto par : m_netSet)
   {
      auto sock = par.first;
      bigone = bigone > sock ? bigone : sock;
      FD_SET(sock, &fs);
      FD_SET(sock, &ferr);
   }

   struct timeval tmInterval;
   tmInterval.tv_sec = 0;
   tmInterval.tv_usec = 0;
   int ret = select(bigone + 1, &fs, 0, &ferr, &tmInterval);
   if (ret > 0)
   {
      LOG_DEBUG("INFO: select positive return, ret %d\n", ret);
      for (auto par : m_netSet)
      {
         auto sock = par.first;
         if (FD_ISSET(sock, &fs))
         {
            receiveDataHandle(sock);
         }
         if (FD_ISSET(sock, &ferr))
         {
            errorHandle(sock);
         }
      }

      for (auto sid : m_ABNSocks)
      {
         m_netSet.erase(sid);
      }
      m_ABNSocks.clear();
   }
   else if (ret < 0)
   {
      LOG_DEBUG("WARNING: select error is happenning, ret %d\n", ret);
   }
}

bool netService::sendData(u32 id, u8 *pt, u32 len, string ip, u16 port)
{
   if (m_netSet.find(id) == m_netSet.end())
      return false;
   if (m_netSet[id]->type == TCP)
   {
      send(id, pt, len, 0);
   }
   else
   { // udp
      sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = inet_addr(ip.c_str());
      sendto(id, pt, len, 0, (struct sockaddr *)&addr, sizeof(addr));
   }
   return true;
}

void netService::receiveDataHandle(u32 sock)
{
   char buf[1024];
   int len;
   sockaddr_in sender_addr;
   u32 sender_addr_size = sizeof(sender_addr);

   if (m_netSet[sock]->type == TCP)
   {
      len = recv(sock, buf, 1024, 0);
   }
   else
   { // udp
      len = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&sender_addr, &sender_addr_size);
   }
   LOG_DEBUG("INFO: Sock %d  receive data : %d\n", sock, len);
   if (len > 0)
   {
      m_netSet[sock]->dataHandler(buf, len);
   }
   else
   { // error happens
      errorHandle(sock);
   }
}
void netService::errorHandle(u32 sock)
{
   LOG_DEBUG("ERROR: Sock %d  error !!!!\n", sock);
   m_ABNSocks.push_back(sock);
   m_netSet[sock]->errorHandler(sock);
}

void netService::close(u32 ide)
{
   if (m_netSet.find(ide) == m_netSet.end())
   {
      shutdown(ide, 2);
      m_netSet.erase(ide);
   }
}

netService *getNetIF()
{
   static netService net;
   return &net;
}