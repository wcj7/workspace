#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <string>

using namespace std;

#define MANUAL_MODE_SAVE_FILE "manual_mode.data"

class Utils
{
public:
    Utils();
    
    /**
     * @brief 获取系统运行时间对应的Seconds
     * @return 系统运行时间对应的Seconds
     */
    static uint32_t getRunTimeSeconds();
    
    /**
     * @brief 获取系统运行时间对应的毫秒
     * @return 系统运行时间对应的毫秒
     */
    static uint64_t getRunTimeMicroSeconds();
    
    /**
     * @brief 毫秒级睡眠，中断后自动重新调用
     * @return
     */
    static void msleep(uint32_t mseconds);
    
    /**
     * @brief 指定位置获取无符号32位整数
     * @param data 数据包地址
     * @param index 指定位置
     */
    static uint32_t getUInt32(uint8_t *data, uint32_t index);
    
    /**
     * @brief 指定位置获取无符号64位整数
     * @param data 数据包地址
     * @param index 指定位置
     */
    static uint64_t getUInt64(uint8_t *data, uint32_t index);
    
    static bool writeManualMode(int mode);
    static bool readManualMode(int &mode);
    
    static bool writeFileInt(string fileName, int number);
    static bool readFileInt(string fileName, int &number);
    
    static string & trim(string &str);

    static string uint8ToHexString(uint8_t *data, int size);
    
    static unsigned char crc8(unsigned char *ptr, int len);
    
    static int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);
    
    
};

#endif // UTILS_H
