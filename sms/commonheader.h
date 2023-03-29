#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

bool isLittleEndian();

u16 netDataToU16(u8*);
u32 netDataToU32(u8*);

void netDataFromU16(u8*, u16);
void netDataFromU32(u8*, u32);