#include "utils.h"

#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

Utils::Utils()
{
    
}

uint32_t Utils::getRunTimeSeconds()
{
    struct timespec runTime = {0, 0};
    uint32_t ret = 0;
    clock_gettime(CLOCK_MONOTONIC, &runTime);
    ret = runTime.tv_sec;
    return ret;
}

uint64_t Utils::getRunTimeMicroSeconds()
{
    struct timespec runTime = {0, 0};
    uint64_t ret = 0;
    clock_gettime(CLOCK_MONOTONIC, &runTime);
    ret = (runTime.tv_sec * 1000) + (runTime.tv_nsec / 1000000);
    return ret;
}

void Utils::msleep(uint32_t mseconds)
{
    struct timespec req = {mseconds / 1000, (mseconds % 1000) * 1000000};
    while ((-1 == nanosleep(&req, &req)) && (EINTR == errno));
}

uint32_t Utils::getUInt32(uint8_t *data, uint32_t index)
{
    uint32_t ret = 0;
    memcpy(&ret, (data + index), sizeof(uint32_t));
    return ret;
}

uint64_t Utils::getUInt64(uint8_t *data, uint32_t index)
{
    uint64_t ret = 0;
    memcpy(&ret, (data + index), sizeof(uint64_t));
    return ret;
}

bool Utils::writeManualMode(int mode)
{
    ofstream file(MANUAL_MODE_SAVE_FILE);
    if (!file.is_open()) {
        cout << "write manual mode failed" << endl;
        return false;
    }
    stringstream sstr;
    sstr << mode;
    file << (sstr.str());
    file.close();
    cout << "write manual mode: " << mode << " success" << endl;
    return true;
}

bool Utils::readManualMode(int &mode)
{
    ifstream file(MANUAL_MODE_SAVE_FILE);
    if (!file.is_open()) {
        cout << "read manual mode failed" << endl;
        return false;
    }
    char str[20];
    file >> str;
    stringstream sstr(str);
    sstr >> mode;
    cout << "read manual mode: " << mode << " success" << endl;
    return true;
}

bool Utils::writeFileInt(string fileName, int number)
{
    ofstream file(fileName.c_str());
    if (!file.is_open()) {
        cout << "write file number failed" << endl;
        return false;
    }
    stringstream sstr;
    sstr << number;
    file << (sstr.str());
    file.close();
    cout << "write file number: " << number << " success" << endl;
    return true;
}

bool Utils::readFileInt(string fileName, int &number)
{
    ifstream file(fileName.c_str());
    if (!file.is_open()) {
        cout << "read file number failed" << endl;
        return false;
    }
    char str[20];
    file >> str;
    stringstream sstr(str);
    sstr >> number;
//    cout << "read file number: " << number << " success" << endl;
    return true;
}

string &Utils::trim(string &str)
{
    string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));
    str.erase(str.begin(), p);
    string::reverse_iterator rp = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int, int>(isspace)));
    str.erase(rp.base(), str.end());
    return str;
}

string Utils::uint8ToHexString(uint8_t *data, int size)
{
    stringstream outPutStr;
    outPutStr.str("");
    for (int i = 0; i < size; i ++) {
        int p = (int)(data[i]);
        outPutStr << std::hex << std::setw(2) << std::setfill('0') << p;
    }
    return outPutStr.str();
}

unsigned char Utils::crc8(unsigned char *ptr, int len)
{
    unsigned char i, crc=0x00; /* 计算的初始crc值 */ 
    while (len--) {
        crc ^= *ptr++;  
        for (i = 8; i > 0; --i) { 
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return (crc);
}

int Utils::set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio;
    if ( tcgetattr( fd,&newtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    //bzero( &newtio, sizeof( newtio ) );
    //newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    /*数据位*/
    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    /*奇偶校验位*/
    switch( nEvent )
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }
    /*波特率*/
    switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 230400:
        cfsetispeed(&newtio, B230400);
        cfsetospeed(&newtio, B230400);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    /*停止位*/
    if( nStop == 1 )
        newtio.c_cflag &= ~CSTOPB;
    
    else if ( nStop == 2 )
        newtio.c_cflag |= CSTOPB;
    
    newtio.c_lflag = 0;
    newtio.c_oflag = 0;
    newtio.c_iflag = 0;
    newtio.c_cc[VEOF] = 0;
    
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);
    
    /*激活配置*/
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
    //printf("Set ttyS0 done!\n");
    return 0;
}
