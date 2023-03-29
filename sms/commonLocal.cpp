#include "commonheader.h"

bool isLittleEndian()
{
    union
    {
        int i;
        char c;
    }un;
    un.i = 1;
    return un.c == 1;
}

u16 netDataToU16(u8* p)
{
    return p[0]<<8|p[1];
}
u32 netDataToU32(u8* p)
{
    return p[0]<<24|p[1]<<16|p[2]<<8|p[3];
}

void netDataFromU16(u8* pbuf, u16 value)
{
    pbuf[0] = (value>>8)&0xff;
    pbuf[1] = value&0xff;
}
void netDataFromU32(u8* pbuf, u32 value)
{
    pbuf[0] = (value>>24)&0xff;
    pbuf[1] = (value>>16)&0xff;
    pbuf[2] = (value>>8)&0xff;
    pbuf[3] = value&0xff;
}