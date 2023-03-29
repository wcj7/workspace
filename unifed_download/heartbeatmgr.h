#ifndef HEARTBEATMGR_H
#define HEARTBEATMGR_H

#include "udpsocket.h"
#include "workthread.h"

#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#define HEARTBEAT_TIMEOUT 200
#define DELAYED_STARTUP_TIME 3000

using namespace std;

/*
 * 只有在isAvailable为true时，isRunning才有可能为true，
 * isRunning为false时，isAvailable一定为false
 */
typedef struct WorkStatus {
    bool isAvailable; ///<是否有效
    bool isRunning;///<是否正在工作
}WorkStatus;

/** 获取程序工作状态，必须实现为非阻塞方式 */
typedef WorkStatus (*getWorkStatus)();
/** 设置程序是否工作，必须实现为非阻塞方式 */
typedef void (*setWorkMode)(bool isWork);


class HeartbeatMgr : public WorkThread
{
public:
    ~HeartbeatMgr();
    
    static HeartbeatMgr &instance();
    
    /*
     * 冗余库初始化，并自动启动线程，不需要再手动调用start()，需要停止时可以手动调用stop()停止线程
     * param getWorkStatusCallback, 获取工作状态回调函数
     * param setWorkModeCallback, 设置工作状态回调函数
     * param isMaster, 主从标识
     * param peerIp, 对端IP地址
     * param peerPort, 通信端口
     * param timeoutCnt, 心跳超时判断阈值，超时时间为: (200 * timeoutCnt)毫秒
     * 
     * return true: success, false: failed
     */
    bool init(getWorkStatus getWorkStatusCallback, setWorkMode setWorkModeCallback, bool isMaster, string peerIp, int peerPort, int timeoutCnt);
    
protected:
    void workThread();
    
private:
    HeartbeatMgr();
    
private:
    static HeartbeatMgr *sMgr;
    
    bool playModeControl();
    
    bool sendHeartbeatPacket();
    
    uint64_t getRunTimeMilliseconds(uint32_t perMilliseconds);
    
    bool isMaster;
    
    int timeoutCount;
    int toutIndex;
    
    UdpSocket heartUdpSocket;
    
    uint64_t startupTimeMilliseconds;
    
    getWorkStatus getStatusCallback;
    setWorkMode setModeCallback;
    WorkStatus localWorkStatus;
    WorkStatus peerWorkStatus;
};

#endif // HEARTBEATMGR_H
