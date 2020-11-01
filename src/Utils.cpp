#include "Utils.h"
#include <algorithm>
#include <Arduino.h>
void getAuthKey(uint8_t in[16],uint8_t out[16],uint8_t key[16]) {
    esp_aes_context ctx;
    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx,key,128);
    esp_aes_crypt_ecb(&ctx,ESP_AES_ENCRYPT,in,out);
}

void getMsgPacket(char* title,char* content,uint8_t* out,int& length) {
    length = strlen(title) + strlen(content) + 3 + 1;
    out[0] = 0xFA;
    out[1] = 0x01;
    out[2] = 0x04;
    std::copy(title,title + strlen(title),out + 3);
    std::copy(content,content + strlen(content),out + 3 + strlen(title) + 1);
    
    log_i("Title Length: %d, Content Length: %d",strlen(title),strlen(content));
    log_printf("[Data Length=%d] ",length);
    for(int i = 0;i < length;++i) {
        log_printf("%02x ",out[i]);
    }
    log_printf("\n");
}