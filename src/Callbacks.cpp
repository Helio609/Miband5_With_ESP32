#include "Callbacks.h"
#include "Utils.h"
NimBLEAdvertisedDevice* advertisedMiband = nullptr;
NimBLEClient* mibandClient = nullptr;
std::string mibandAddress = "14:08:06:04:2a:52";
uint8_t key[16] = {0xAC,0x7C,0xAB,0x18,0xC4,0x7B,0x14,0xB6,
                   0x6F,0x4C,0x82,0x2B,0xFF,0x3D,0x43,0xC1};

void ClientCallbacks::onConnect(BLEClient *pClient) {
    log_i("成功连接到蓝牙设备");
}

void ClientCallbacks::onDisconnect(BLEClient *pClient) {
    log_i("蓝牙设备断开连接");
    advertisedMiband = nullptr;
    BLEDevice::getScan()->start(5,false);
}

bool ClientCallbacks::onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params) {
    return true;
}

uint32_t ClientCallbacks::onPassKeyRequest() {
    return 123456;
}

void ClientCallbacks::onAuthenticationComplete(ble_gap_conn_desc* desc) {
    if(!desc->sec_state.encrypted) {
        log_i("加密连接失败");
        NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
        return;
    }
}

bool ClientCallbacks::onConfirmPIN(uint32_t pin) {
    log_i("认证PIN: %d",pin);
    return true;
}

void AdvertisedDeviceCallbacks::onResult(NimBLEAdvertisedDevice* advertisedDevice) {
    log_i("%s",advertisedDevice->getAddress().toString().c_str());
    BLEAddress mibandAddr(mibandAddress);
    if (advertisedDevice->getAddress().equals(mibandAddr)) {
        log_i("找到小米手环，尝试连接");
        BLEDevice::getScan()->stop();
        advertisedMiband = new NimBLEAdvertisedDevice(*advertisedDevice);
    }
}

void MibandNotifyCallbacks::authNotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify) {
    log_i("手环认证回调");
    log_printf("[Data Length=%d] ",length);
    
    for(int i = 0;i < length;++i) {
        log_printf("%02x ",pData[i]);
    }
    log_printf("\n");
    if(length == 20 && pData[0] == 0x10 && pData[1] == 0x01 && pData[2] == 0x81) {
        uint8_t reqRndNun[] = {0x82,0x00,0x02};
        pBLERemoteCharacteristic->writeValue(reqRndNun);
    }else if(length == 19 && pData[0] == 0x10 && pData[1] == 0x82 && pData[2] == 0x01) {
        uint8_t inbuf[16] = {0};
        uint8_t outbuf[16] = {0};
        for(int i = 0 ; i < 16 ; ++i) {
            inbuf[i] = pData[i + 3];
        }
        getAuthKey(inbuf,outbuf,key);
        uint8_t rspAuthkey[18] = {0};
        rspAuthkey[0] = 0x83;
        rspAuthkey[1] = 0x00;
        for(int i = 0 ; i < 16 ; ++i) {
            rspAuthkey[i + 2] = outbuf[i];
        }
        pBLERemoteCharacteristic->writeValue(rspAuthkey);
    }else if(length == 3 && pData[0] == 0x10 && pData[1] == 0x83 && pData[2] == 0x01) {
        log_i("手环认证成功");
    }else if(length == 3 && pData[0] == 0x10 && pData[1] == 0x83 && pData[2] == 0x08) {
        log_i("手环认证失败");
        uint8_t reqRndNun[] = {0x82,0x00,0x02};
        pBLERemoteCharacteristic->writeValue(reqRndNun);
    }
}

void MibandNotifyCallbacks::statusNotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify) {
    log_i("手环状态回调");
    log_printf("[Data Length=%d] ",length);
    
    for(int i = 0;i < length;++i) {
        log_printf("%02x ",pData[i]);
    }
    log_printf("\n");

    uint8_t low = pData[2];
    uint8_t high = pData[1];
    int val = (high & 0x0000ffff) * pow(16,0) + (high & 0xffff0000) * pow(16,1)
            + (low  & 0x0000ffff) * pow(16,2) + (low  & 0xffff0000) * pow(16,3);
    log_i("当前步数: %d步",val);

    low = pData[10];
    high = pData[9];
    val = (high & 0x0000ffff) * pow(16,0) + (high & 0xffff0000) * pow(16,1)
        + (low  & 0x0000ffff) * pow(16,2) + (low  & 0xffff0000) * pow(16,3);
    log_i("当前消耗: %d千卡",val);

    low = pData[6];
    high = pData[5];
    val = (high & 0x0000ffff) * pow(16,0) + (high & 0xffff0000) * pow(16,1)
        + (low  & 0x0000ffff) * pow(16,2) + (low  & 0xffff0000) * pow(16,3);
    log_i("当前距离: %d千米",val / 1000);
}

void MibandNotifyCallbacks::heartRateNotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify) {
    log_i("手环心率回调");
    log_printf("[Data Length=%d] ",length);
    
    for(int i = 0;i < length;++i) {
        log_printf("%02x ",pData[i]);
    }
    log_printf("\n");

    uint8_t val = pData[1];
    log_i("当前心率: %dbpm",val);
}

void MibandNotifyCallbacks::voiceControlChar1NotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify) {
    log_i("手环语音回调-1");
    log_printf("[Data Length=%d] ",length);
    
    for(int i = 0;i < length;++i) {
        log_printf("%02x ",pData[i]);
    }
    log_printf("\n");
}

void MibandNotifyCallbacks::voiceControlChar2NotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify) {
    log_i("手环语音回调-2");
    log_printf("[Data Length=%d] ",length);
    
    for(int i = 0;i < length;++i) {
        log_printf("%02x ",pData[i]);
    }
    log_printf("\n");
}

void MibandNotifyCallbacks::voiceControlChar3NotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify) {
    log_i("手环语音回调-3");
    log_printf("[Data Length=%d] ",length);
    
    for(int i = 0;i < length;++i) {
        log_printf("%02x ",pData[i]);
    }
    log_printf("\n");
}