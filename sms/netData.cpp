#include "log.h"
#include "netData.h"
#include "xmlServer.h"
#include "taskManager.h"
#include <sstream>
#include <string>
#include <functional>
#include <map>
#include <memory>
#include "configs.h"
using namespace std;

const string xmlHead = R"(<?xml version="1.0" encoding="UTF-8"?>)";
const u32 siementHeadLen = 31;
const u32 timeLen = 12;

static time_t timeToSeconds(int y, int m, int d, int ht = 0, int hm = 0, int hs = 0)
{
  tm tmp = {
      .tm_sec = hs,
      .tm_min = hm,
      .tm_hour = ht,
      .tm_mday = d,
      .tm_mon = m - 1,
      .tm_year = y - 1900,
      .tm_wday = 0,
      .tm_yday = 0,
      .tm_isdst = 0};
  return mktime(&tmp);
}

/*
 *  Transform the Txt message retrived from xml to struct for later scheduling
 *   Set err in the struct to be -1 when parse error happens
 */
static shared_ptr<MSG> generateMsg(map<string, string> &info)
{
  auto pMsg = make_shared<MSG>();
  pMsg->err = 0;

  for (auto &d : info)
  {
    if (d.second == "")
    {
      LOG_DEBUG("ERROR: Parse xml element failure ... %s\n", d.first.c_str());
      pMsg->err = -1;
      return pMsg;
    }
  }
  auto s = info["start"];
  auto e = info["end"];
  try
  {
    pMsg->ctr.interv = stoi(info["interv"]);
    pMsg->ctr.repeat = stoi(info["RepeatMsg"]);
    pMsg->ctr.syncAudio = stoi(info["AudioSync"]);
    pMsg->ctr.scroll = stoi(info["ScrollMsg"]);
    pMsg->ctr.priority = stoi(info["OverlayPriority"]);

    pMsg->data.msgId = stoi(info["MsgID"]);
    pMsg->data.msgLang = stoi(info["MsgLang"]);
    pMsg->data.txtMsg1 = info["TxtMsg1"];
    pMsg->data.txtMsg2 = info["TxtMsg2"];
    pMsg->data.txtMsg3 = info["TxtMsg3"];

    if (s.length() == timeLen && e.length() == timeLen)
    { // Time formate is YYYYMMDDHHMM, with fixed length
      pMsg->ctr.start = timeToSeconds(stoi(s.substr(0, 4)), stoi(s.substr(4, 2)), stoi(s.substr(6, 2)), stoi(s.substr(8, 2)), stoi(s.substr(10, 2)));
      pMsg->ctr.end = timeToSeconds(stoi(e.substr(0, 4)), stoi(e.substr(4, 2)), stoi(e.substr(6, 2)), stoi(e.substr(8, 2)), stoi(e.substr(10, 2)));
    }
    else if (s == "0" && e == "0")
    { // The message should be removed
      pMsg->ctr.start = 0;
      pMsg->ctr.end = 0;
      return pMsg;
    }
    else
    {
      LOG_DEBUG("ERROR: Parse xml time failure %s:%s \n", s.c_str(), e.c_str());
      pMsg->err = -1;
      return pMsg;
    }
  }
  catch (...)
  {
    LOG_DEBUG("ERROR: xml content not match the requirment, exception hanpen in string to int\n");
    pMsg->err = -1;
    return pMsg;
  }

  if (pMsg->ctr.start < 0 || pMsg->ctr.end < 0)
  {
    LOG_DEBUG("ERROR: Parse xml time failure2 %d:%d \n", pMsg->ctr.start, pMsg->ctr.end);
    pMsg->err = -1;
    return pMsg;
  }

  time_t timep;
  time(&timep);
  if (timep > pMsg->ctr.end || pMsg->ctr.end < pMsg->ctr.start)
  { // current time is bigger than end time, or start time is bigger than end time
    LOG_DEBUG("ERROR: Parse time is invalid %s:%s \n", s.c_str(), e.c_str());
    pMsg->err = 4;
  }
  else if (timep > pMsg->ctr.start)
  { // The start time has passed, adjust the begin time properly
    LOG_DEBUG("WARNING: The Message is a little late, adjust begin time from  %d to %d \n", pMsg->ctr.start, timep);
    pMsg->ctr.start = timep;
  }

  return pMsg;
}

tcpData::tcpData(netService *p) : mpnet(p)
{
  mState = INIT;
  mpbuf = (char *)malloc(1024);
  mLen = 0;
  mInterval = GVALUETOI(reconnect_interval);
}

/*
 *  We have all the raw data coming from SIEMENTS server here, assume that it will receive 2 kinds message
 *  1. ACK:  After sending Train No information to SIEMENTS, server should ACK it
 *  2. TXT:  SIEMENTS send messages to PIS to display,  should include two xml, command and message
 */
void tcpData::onData(char *pdata, int len)
{
  memcpy(mpbuf + mLen, pdata, len);
  mLen += len;
  if (mLen < siementHeadLen)
  {
    return;
  }
  packHead ph;
  bool br = unPackMsgHead(mpbuf, &ph);
  LOG_DEBUG("INFO: unpack info %d:%d:%d --- %d\n", ph.msgSize, ph.packSize, ph.phead, mLen);
  if (ph.packSize <= mLen)
  {
    bool bret = false;
    xmlServer ser;
    switch (ph.type[0])
    {
    case 'A': // ACK from SIMENTS server
    {
      map<string, string> sack = {{"Sequence", ""}, {"ErrorCode", ""}};
      ser.loadString(mpbuf + siementHeadLen);
      ser.parseMsg(sack);
      LOG_DEBUG("INFO: Get ACK from SIEMENTS ack: %s", sack["ErrorCode"].c_str());
      if (sack["ErrorCode"] == "0") // 0 indicate success for ack information
      {
        mState = TRAINACK;
      }
    }
    break;

    case 'T': // TXT from SIEMENTS server
    {
      char *p1 = strstr(mpbuf + siementHeadLen + xmlHead.length(), xmlHead.c_str());
      int rev = 0;
      if (p1 != NULL)
      {
        string cmdstr(mpbuf + siementHeadLen, p1 - mpbuf - siementHeadLen); // shchedul information
        string msgstr(p1, ph.msgSize - cmdstr.length());                    // msg body
        xmlServer ser2;
        map<string, string> msg = {{"start", ""}, {"end", ""}, {"interv", ""}, {"RepeatMsg", ""}, {"AudioSync", ""}, {"ScrollMsg", ""}, {"OverlayPriority", ""}, {"MsgID", ""}, {"TxtMsg1", ""}, {"TxtMsg2", ""}, {"TxtMsg3", ""}, {"MsgLang", ""}};
        ser.loadString(cmdstr.c_str());
        ser.parseMsg(msg);
        ser2.loadString(msgstr.c_str());
        ser2.parseMsg(msg);
        auto pMsg = generateMsg(msg);
        rev = pMsg->err == -1 ? 1 : pMsg->err;
        if (rev == 0)
        { //
          getTaskManager()->executeNewMsg(pMsg);
        }
      }
      else
      {
        rev = 1;
        LOG_DEBUG("ERROR: failed to find the msg xmlHead\n");
      }
      sendACK(ph.sq, rev);
    }
    break;
    default:
    {
      stringstream data;
      data << ph.type[0] << ph.type[1] << ph.type[2];
      LOG_DEBUG("WARNING: unknow Type message receive : %s \n", data.str());
    }
    }
    mLen -= ph.packSize;
    memcpy(mpbuf, mpbuf + ph.packSize, mLen);
  }
}

void tcpData::onError(int e)
{
  mTimePoint = chrono::system_clock::now();
  mState = ERRHAPPEN;
  LOG_DEBUG("Error: TCP is in error %s:%d\n", mIp.c_str(), mport);
}

bool tcpData::connectServer(string ip, u16 port)
{
  LOG_DEBUG("INFO: connect server  %s: %d\n", ip.c_str(), port);
  if (mState == CONNECTED)
  {
    mpnet->close(mId);
  }
  mTimePoint = std::chrono::system_clock::now();
  mId = mpnet->createTCPClient(ip, port);
  mIp = ip;
  mport = port;
  if (mId != 0)
  {
    mState = CONNECTED;
    mpnet->setHandler(mId, bind(&tcpData::onData, this, placeholders::_1, placeholders::_2),
                      bind(&tcpData::onError, this, placeholders::_1));
  }
  else
  {
    LOG_DEBUG("Error: connect failed \n");
    mState = ERRHAPPEN;
  }

  return mId != 0;
}

bool tcpData::tryReconnct()
{
  if (mState != CONNECTED)
  {
    auto tp = chrono::system_clock::now();
    if (chrono::duration_cast<chrono::seconds>(tp - mTimePoint).count() >= mInterval)
    {
      mTimePoint = tp;
      return connectServer(mIp, mport);
    }
  }
  return false;
}

void tcpData::sendTrainInfo(string tno)
{
  if (mState != CONNECTED)
  {
    LOG_DEBUG("WARNING: Not ready for sending\n");
    return;
  }
  static chrono::system_clock::time_point tp;
  if (chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - tp).count() <= 5) // send interval should bigger than 5s
  {
    return;
  }
  tp = chrono::system_clock::now();
  LOG_DEBUG("INFO: Send train No: %s\n", tno.c_str());
  xmlServer xmlser;
  string xmlStr = xmlser.generateTrainNo(tno);
  u8 buf[] = {'I', 'N', 'F'};
  packHead head = packMsg(0xff, 0, buf, xmlStr);
  mpnet->sendData(mId, (u8 *)&head, siementHeadLen);
  mpnet->sendData(mId, (u8 *)(xmlStr.c_str()), xmlStr.length());
}

void tcpData::sendACK(u16 sq, int errCode)
{
  if (mState < CONNECTED)
  {
    LOG_DEBUG("WARNING: Not ready for sending\n");
    return;
  }
  LOG_DEBUG("INFO: Send ACK Sq: %d, Errcode: %d\n", sq, errCode);
  xmlServer xmlser;
  string xmlStr = xmlser.generateACK(sq, errCode);
  u8 buf[] = {'A', 'C', 'K'};
  packHead head = packMsg(0xff, sq, buf, xmlStr);
  mpnet->sendData(mId, (u8 *)&head, siementHeadLen);
  mpnet->sendData(mId, (u8 *)(xmlStr.c_str()), xmlStr.length());
}

bool udpData::startServerPoint(string ip, u16 port)
{
  mId = mpnet->createUDPPoint(ip, port);
  mIp = ip;
  mport = port;
  if (mId != 0)
  {
    mpnet->setHandler(mId, bind(&udpData::onData, this, placeholders::_1, placeholders::_2),
                      bind(&udpData::onError, this, placeholders::_1));
  }
  else
  {
    LOG_DEBUG("Error: Bind port failed \n");
  }
  return mId != 0;
}

void udpData::onData(char *pdata, int len)
{
  mTrainNo.assign(pdata, pdata + len);
  LOG_DEBUG("INFO: udp receive data length %d , trainNo: %s\n", len, mTrainNo.c_str());
}
void udpData::onError(int e)
{
  LOG_DEBUG("Warning: For udp error, nothing to do, just reminder \n");
}
void udpData::sendData(const char *pbuf, int len, string tIp, u16 tport)
{
  LOG_DEBUG("INFO: Udp send to %s:%d \n", tIp.c_str(), tport);
  mpnet->sendData(mId, (u8 *)pbuf, len, tIp, tport);
}