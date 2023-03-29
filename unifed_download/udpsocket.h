#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>

using namespace std;

/*
 * 实现Udp发送接收功能
 * 1、peerIp,peerPort可以为组播地址，也可以为单播地址，通过判断peerIp是否为组播地址，来决定用于组播通信或单播通信
 * 2、组播发送接收：localIp, peerIp, peerPort
 * 3、单播发送接收：localPort, peerIp, peerPort
 */
class UdpSocket
{
public:
    UdpSocket();
    ~UdpSocket();
    
    /*
     * 组播：peer表示组播发送接收地址，localIp表示在指定接口上加入组播，localPort不适用
     * 单播：peer表示单播对端地址，localIp不适用，localPort表示本地绑定接收端口
    */
    bool init(string localIp, int localPort, string peerIp, int peerPort, int sendBufSize, int recvBufSize, int millisecondTimeout);
    bool reinit();
    int send(unsigned char *buf, int size);
    int recv(unsigned char *buf, int size);
    
    int getSockfd() {return sockfd;}
    
public:
    void close();
    
private:
    int sockfd;
    bool mIsMuiltcast;
    string mLocalIp;
    int mLocalPort;
    string mPeerIp;
    int mPeerPort;
    struct sockaddr_in sendAddr;
    int mSendBufSize;
    int mRecvBufSize;
    int mTimeout;
};

#endif // UDPSOCKET_H
