#include "xmlServer.h"
#include <stdio.h>
#include <cstring>
#include "commonheader.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include "log.h"
using namespace std;

// indicate the local machine
static bool isLittleEndians = true;

// should be invake in the first place
void initiateStaticLocal()
{
  isLittleEndians = isLittleEndian();
}

packHead packMsg(u32 imak, u16 sq, u8 *ptype, string msg)
{
  packHead head;
  netDataFromU32((u8 *)&head.phead, imak);
  netDataFromU32((u8 *)&head.packSize, 31 + msg.length());
  netDataFromU32((u8 *)&head.msgSize, msg.length());
  netDataFromU16((u8 *)&head.sq, sq);
  memcpy(&head.type, ptype, 3);
  time_t now = time(0);
  tm *ltm = localtime(&now);
  string localtm = to_string(1900 + ltm->tm_year);
  localtm += 1 + ltm->tm_mon > 9 ? to_string(1 + ltm->tm_mon) : "0" + to_string(1 + ltm->tm_mon);
  localtm += ltm->tm_mday > 9 ? to_string(ltm->tm_mday) : "0" + to_string(ltm->tm_mday);
  localtm += ltm->tm_hour > 9 ? to_string(ltm->tm_hour) : "0" + to_string(ltm->tm_hour);
  localtm += ltm->tm_min > 9 ? to_string(ltm->tm_min) : "0" + to_string(ltm->tm_min);
  localtm += ltm->tm_sec > 9 ? to_string(ltm->tm_sec) : "0" + to_string(ltm->tm_sec);
  memcpy(head.timeStamp, localtm.c_str(), 14);
  return head;
}

bool unPackMsgHead(void *buf, packHead *pHead)
{
  memcpy(pHead, buf, sizeof(packHead));
  if (isLittleEndians)
  {
    pHead->phead = netDataToU32((u8 *)buf);
    pHead->msgSize = netDataToU32((u8 *)&pHead->msgSize);
    pHead->packSize = netDataToU32((u8 *)&pHead->packSize);
    pHead->sq = netDataToU16((u8 *)&pHead->sq);
  }
  return true;
}

bool xmlServer::loadString(const char *str)
{
  return doc.Parse(str, 0, TIXML_ENCODING_UTF8) != 0;
}

static void GetEleValue(TiXmlElement *element, map<string, string> &sets)
{
  for (TiXmlElement *currentele = element->FirstChildElement(); currentele; currentele = currentele->NextSiblingElement())
  {
    // XMLElement *tmpele = currentele;
    if (currentele->Value() != NULL)
    {
      if (sets.find(currentele->Value()) != sets.end())
      {
        sets[currentele->Value()] = currentele->GetText();
      }
    }

    if (!currentele->NoChildren())
      GetEleValue(currentele, sets);
  }
}
void xmlServer::parseMsg(map<string, string> &sets)
{
  TiXmlElement *root = doc.FirstChildElement();
  if (root)
  {
    GetEleValue(root, sets);
  }
}

string xmlServer::generateTrainNo(string &trainNo)
{
  ifstream f;
  string all, line;
  f.open("config/init.xml", ios::in);
  if (f.is_open())
  {
    while (!f.eof())
    {
      f >> line;
      all += line;
    }
    string::size_type pos(0);
    if ((pos = all.find("RETID", pos)) != string::npos)
    {
      all.replace(pos, 5, trainNo);
    }
    f.close();
  }
  return all;
}

string xmlServer::generateACK(u16 seq, int err)
{
  ifstream f;
  string all, line;
  f.open("config/ack.xml", ios::in);
  if (f.is_open())
  {
    while (!f.eof())
    {
      f >> line;
      all += line;
    }
    string::size_type pos(0);
    if ((pos = all.find("SREPLACE", pos)) != string::npos)
    {
      all.replace(pos, 8, to_string(seq));
    }
    pos = 0;
    if ((pos = all.find("EREPLACE", pos)) != string::npos)
    {
      all.replace(pos, 8, to_string(err));
    }
    f.close();
  }
  return all;
}
