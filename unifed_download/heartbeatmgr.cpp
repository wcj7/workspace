#include "heartbeatmgr.h"

#include "log.h"

#include <time.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <cstddef>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW 4
#endif

HeartbeatMgr *HeartbeatMgr::sMgr = NULL;

HeartbeatMgr::HeartbeatMgr() : heartUdpSocket()
{
    toutIndex = 0;
    localWorkStatus.isAvailable = false;
    localWorkStatus.isRunning = false;
    peerWorkStatus.isAvailable = false;
    peerWorkStatus.isRunning = false;
}

HeartbeatMgr &HeartbeatMgr::instance()
{
    if (NULL == sMgr) {
        sMgr = new HeartbeatMgr();
    }
    return (*sMgr);
}

bool HeartbeatMgr::init(getWorkStatus getWorkStatusCallback, setWorkMode setWorkModeCallback, bool isMaster, string peerIp, int peerPort, int timeoutCnt)
{
    getStatusCallback = getWorkStatusCallback;
    setModeCallback = setWorkModeCallback;
    this->isMaster = isMaster;
    timeoutCount = timeoutCnt;
    if (!heartUdpSocket.init("", peerPort, peerIp, peerPort, 0, 0, HEARTBEAT_TIMEOUT)) {
        return false;
    }
    
    LOG_DEBUG("HeartbeatMgr::init timeout: %dms\n", (HEARTBEAT_TIMEOUT * timeoutCnt)); 
    
    return start();
}

void HeartbeatMgr::workThread()
{
    LOG_DEBUG("Heartbeat thread started...\n");;
    struct timeval timeVal;
    fd_set readSets;
    unsigned char buf[10];
    
    startupTimeMilliseconds = getRunTimeMilliseconds(1);
    
    while (isRun) {
        if (!heartUdpSocket.reinit()) {
            sleep(1);
            continue;
        }
        
        LOG_DEBUG("Init heartbeat socket success\n");
        toutIndex = timeoutCount;
        timeVal.tv_sec = HEARTBEAT_TIMEOUT / 1000;
        timeVal.tv_usec = (HEARTBEAT_TIMEOUT % 1000) * 1000;
        while (isRun) {
            FD_ZERO(&readSets);
            FD_SET(heartUdpSocket.getSockfd(), &readSets);
            int ret = select((heartUdpSocket.getSockfd() + 1), &readSets, NULL, NULL, &timeVal);
            if (ret < 0) {
                toutIndex = timeoutCount;
                peerWorkStatus.isAvailable = false;
                peerWorkStatus.isRunning = false;
                break;
            } else if (0 == ret) {
                //timeout
                if (toutIndex > 0) {
                    toutIndex --;
                } else {
                    toutIndex = timeoutCount;
                    peerWorkStatus.isAvailable = false;
                    peerWorkStatus.isRunning = false;
                }
            } else {
                if (FD_ISSET(heartUdpSocket.getSockfd(), &readSets)) {
                    int recvLen = heartUdpSocket.recv(buf, sizeof(buf));
                    toutIndex = timeoutCount;
                    peerWorkStatus.isAvailable = buf[2] ? true : false;
                    peerWorkStatus.isRunning = buf[3] ? true : false;
                }
            }
            
            if (0 == timeVal.tv_sec && 0 == timeVal.tv_usec) {
                timeVal.tv_sec = HEARTBEAT_TIMEOUT / 1000;
                timeVal.tv_usec = (HEARTBEAT_TIMEOUT % 1000) * 1000;
                playModeControl();
                if (!sendHeartbeatPacket()) {
                    break;
                }
            }
        }
    }
}

/*
 * 冗余切换机制为默认主端机制，即只要主端available，则主端工作
 * 主从两端都需要先获取主从两端的工作状态
 * available简称AT，not available简称AF；
 * running简称RT，not running简称RF；
 * 1）主AT，从RT，主端停止，从端停止；
 * 2）主AT，从RF，主端启动，从端停止；
 * 3）主AF，从AT，主端停止，从端启动；
 * 4）主AF，从AF，主端停止，从端停止；
 */
bool HeartbeatMgr::playModeControl()
{
    //获取工作状态
    uint64_t currRunTimeMilliseconds = getRunTimeMilliseconds(1);
    localWorkStatus = getStatusCallback();
    if (isMaster) {
        //主端
        if (localWorkStatus.isAvailable && (!peerWorkStatus.isRunning)) {
            if (currRunTimeMilliseconds > (startupTimeMilliseconds + DELAYED_STARTUP_TIME)) {
//                LOG_DEBUG("HeartbeatMgr: master available, turn to work...\n");
                setModeCallback(true);
            } else {
                setModeCallback(false);
            }
        } else {
//            LOG_DEBUG("HeartbeatMgr: master not available, turn to sleep...\n");
            setModeCallback(false);
        }
    } else {
        //从端
        if (localWorkStatus.isAvailable && (!peerWorkStatus.isAvailable)) {
            if (currRunTimeMilliseconds > (startupTimeMilliseconds + DELAYED_STARTUP_TIME)) {
                setModeCallback(true);
            } else {
                setModeCallback(false);
            }
        } else {
            setModeCallback(false);
        }
    }
    return true;
}


//头: 0xb0 0xf0 + 有效状态 + 实际工作状态 + 尾:0xe0
bool HeartbeatMgr::sendHeartbeatPacket()
{
    uint8_t buf[5] = {0xB0, 0xF0, 0x00, 0x00, 0xE0};
    buf[2] = (localWorkStatus.isAvailable ? 1 : 0);
    buf[3] = (localWorkStatus.isRunning ? 1 : 0);
    if (heartUdpSocket.send(buf, sizeof(buf)) < 0) {
        return false;
    }
    
    return true;
}

uint64_t HeartbeatMgr::getRunTimeMilliseconds(uint32_t perMilliseconds)
{
    struct timespec runTime = {0, 0};
    uint64_t ret = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &runTime);
    ret = (runTime.tv_sec * 1000) + (runTime.tv_nsec / 1000000);
    ret = ret / perMilliseconds;
    return ret;
}
