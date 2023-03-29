#pragma once
#include "commonheader.h"
#include <string>
#include <tinyxml.h>
#include <chrono>
#include <map>
using namespace std;

class xmlServer
{
public:
   bool loadString(const char *str);
   void parseMsg(map<string, string> &);
   string generateTrainNo(string &trainNo);
   string generateACK(u16 seq, int err);

private:
   TiXmlDocument doc;
};

struct packHead
{
   u32 phead;
   u32 packSize;
   u16 sq;
   u32 msgSize;
   u8 timeStamp[14];
   u8 type[3];
} __attribute__((packed)); //__attribute__ ((aligned (1)));

packHead packMsg(u32 imak, u16 sq, u8 *ptype, string msg);
bool unPackMsgHead(void *buf, packHead *pHead);
void initiateStaticLocal();
