#include "udpsocket.h"
#include "string.h"
#include "unistd.h"
#include "stdio.h"
#include <errno.h>
#include <iostream>


UdpSocket::UdpSocket()
{
    sockfd = -1;
}

UdpSocket::~UdpSocket()
{
    close();
}

/*
 * 组播：peer表示组播发送接收地址，localIp表示在指定接口上加入组播，localPort不适用
 * 单播：peer表示单播对端地址，localIp不适用，localPort表示本地绑定接收端口
*/
bool UdpSocket::init(string localIp, int localPort, string peerIp, int peerPort, int sendBufSize, int recvBufSize, int millisecondTimeout)
{
    mLocalIp = localIp;
    mLocalPort = localPort;
    mPeerIp = peerIp;
    mPeerPort = peerPort;
    in_addr_t addr = ntohl(inet_addr(mPeerIp.c_str()));
    mIsMuiltcast = (addr >= 0xE0000000 && addr <= 0xEFFFFFFF);
    memset(&sendAddr, 0, sizeof(sendAddr));
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_addr.s_addr = inet_addr(mPeerIp.c_str());
    sendAddr.sin_port = htons(mPeerPort);
    mSendBufSize = sendBufSize;
    mRecvBufSize = recvBufSize;
    //转换为毫秒
    mTimeout = millisecondTimeout;
    
    return reinit();
}

bool UdpSocket::reinit()
{
    struct sockaddr_in localAddr;
    struct ip_mreq mreq;
    socklen_t localAddrLen = sizeof(localAddr);
    
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (mIsMuiltcast) {
        localAddr.sin_port = htons(mPeerPort);
    } else {
        localAddr.sin_port = htons(mLocalPort);
    }
    
    close();
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("init udp socket error");
        sockfd = -1;
        return false;
    }
    
    if (mSendBufSize > 0) {
        if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &mSendBufSize, sizeof(mSendBufSize)) < 0) {
            perror("set send buf option error");
            close();
            return false;
        }
    }
    
    if (mRecvBufSize > 0) {
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &mRecvBufSize, sizeof(mRecvBufSize)) < 0) {
            perror("set recv buf option error");
            close();
            return false;
        }
    }
    
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("set reuse addr option error");
        close();
        return false;
    }
    
    if (mTimeout > 0) {
        int sec = mTimeout / 1000;
        int msec = mTimeout % 1000;
        struct timeval tVal = {sec, (msec * 1000)};
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tVal, sizeof(tVal)) < 0) {
            perror("set recv timeout option error");
            close();
            return false;
        }
    }
    
    if (mIsMuiltcast) {
        mreq.imr_multiaddr.s_addr = inet_addr(mPeerIp.c_str());
        if (!mLocalIp.empty()) {
            mreq.imr_interface.s_addr = inet_addr(mLocalIp.c_str());
        } else {
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        }
        if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
            cout << "errno: " << errno << endl;
            perror("set add membership option error");
            close();
            return false;
        }
        
        if (!mLocalIp.empty()) {
            struct in_addr localAddr;
            localAddr.s_addr = inet_addr(mLocalIp.c_str());
            if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &localAddr, sizeof(localAddr)) < 0) {
                perror("set ip_multicast_if option error");
                close();
                return false;
            }
        }
    }
    
    if (bind(sockfd, (struct sockaddr *)&localAddr, localAddrLen) < 0) {
        perror("bind local addr error");
        close();
        return false;
    }
    
    return true;
}

int UdpSocket::send(unsigned char *buf, int size)
{
    int len = sendto(sockfd, buf, size, 0, (struct sockaddr *)&sendAddr, sizeof(sendAddr));
    if (len < 0) {
        perror("udp send data error");
        close();
        return -1;
    }
    
    return len;
}

int UdpSocket::recv(unsigned char *buf, int size)
{
    if (NULL == buf) {
        cout << "recv buf is null" << endl;
        return -1;
    }
    
    int recvLen = 0;
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    
    recvLen = recvfrom(sockfd, buf, size, 0, (struct sockaddr *)&addr, &addrLen);
    if (recvLen < 0) {
        if (EAGAIN == errno) {
            return 0;
        } else {
            perror("udp recv data error");
            close();
            return -1;
        }
    }
    
    return recvLen;
}

void UdpSocket::close()
{
    if (sockfd >= 0) {
        ::close(sockfd);
        sockfd = -1;
    }
}
