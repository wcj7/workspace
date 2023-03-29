#pragma once
#include "commonheader.h"
#include <string>
#include "netServer.h"
#include <chrono>
using namespace std;

// class  dataHandlerIf
// {
//    public:
//    virtual void handle(u8* pd, int len) = 0;
// };
// class  errorHandlerIf
// {
//     public:
//     virtual void handle(int len) = 0;
// };

// time_t std::chrono::system_clock::to_time_t(const time_point &tp);

// time_point std::chrono::system_clock::from_time_t(time_t t)
struct ScheduleINFO
{
    time_t start;
    time_t end;
    u32 interv;
    u32 repeat;

    u32 syncAudio : 1;
    u32 scroll : 1;
    u32 priority : 2;
    u32 reserved : 28;
};
struct MsgINFO
{
    u16 msgId;
    u16 msgLang;
    string txtMsg1;
    string txtMsg2;
    string txtMsg3;
};

struct MSG // SIEMENTS xml message will tranforms as the struct
{
    ScheduleINFO ctr;
    MsgINFO data;
    int err; // 0 indicate no err, or will be dismissed for no scheduling
};

enum STATESCON
{
    INIT,
    ERRHAPPEN,
    CONNECTED, // Every thing is working on this state and after
    TRAINACK   // After send train number to SIEMENTS, get ACK information from it
};

/*
 *  Operate the link to SIEMENTS server with it
 *
 */

class tcpData
{
    STATESCON mState;
    u32 mId;                                     // socket
    u32 mInterval;                               // reconnect interval, in seconds
    string mIp;                                  // server IP
    u16 mport;                                   // server port
    netService *mpnet;                           // lower layer service object
    char *mpbuf;                                 // buffer to raw data
    int mLen;                                    // data length has been buffered
    chrono::system_clock::time_point mTimePoint; // last time point to reconnect or disconnected

public:
    tcpData(netService *p);
    ~tcpData() { free(mpbuf); }

    void onData(char *pdata, int len);
    void onError(int e);

    inline u32 getState() { return mState; }
    bool connectServer(string ip, u16 port);
    bool tryReconnct();
    void sendTrainInfo(string tno);
    void sendACK(u16 sq, int errCode);
};

typedef struct
{
    u8 Start; /* 0xFA */
    u8 Ver : 7;
    u8 Couple : 1;
    u16 SrcAddr; /* IP:  0x01 0x1E   or 0x01 0x23 */
    u16 DstAddr; /* IP:  0x01 0x1E   or 0x01 0x23 */
    u8 FunIndex; /* tFunSn:  SET_REQ					= 0x07, */
    u16 Length;
    u8 Crc;
    u8 Data[1];
} tPisFrameStruct;

/* u8  Data[1]======SET: SET_OP */
typedef struct
{
    u8 fix;      /* tFixType: SET_OPT, */
    u8 Type : 4; /* tOpType: OPT_XMZ_EM, */
    u8 Opt : 4;  /* tDevOp: OP_START */
    u8 Id;       /* get --XMZ--val  */
    u8 Times;    /* get --XMZ--val  */
} tSetOpt;

// link to the PA, forword information to PA
class udpData
{
    u32 mId;
    string mIp;
    u16 mport;
    netService *mpnet;
    char *mpbuf;
    string mTrainNo;

public:
    udpData(netService *p) : mpnet(p)
    {
        mTrainNo = "";
        mId = 0;
        mpbuf = (char *)malloc(1024);
    }
    ~udpData() { free(mpbuf); }

    bool startServerPoint(string ip, u16 port);
    void onData(char *pdata, int len);
    void onError(int e);
    void sendData(const char *pbuf, int len, string tIp, u16 tport);
    inline string getTrainNo() { return mTrainNo; }
};