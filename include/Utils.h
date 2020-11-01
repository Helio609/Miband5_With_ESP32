#ifndef __UTILS_H
#define __UTILS_H
#include <mbedtls/aes.h>

extern void getAuthKey(uint8_t in[16],uint8_t out[16],uint8_t key[16]);
extern void getMsgPacket(char* title,char* content,uint8_t* out,int& length);

#endif