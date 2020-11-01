#ifndef __CALLBACKS_H
#define __CALLBACKS_H
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "Config.h"

class MibandNotifyCallbacks {
  public:
	static void authNotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify);
  	static void statusNotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify);
  	static void heartRateNotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify);
  	static void voiceControlChar1NotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify);
  	static void voiceControlChar2NotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify);
  	static void voiceControlChar3NotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic,uint8_t *pData,size_t length,bool isNotify);
};

class ClientCallbacks : public NimBLEClientCallbacks {
  public:
	void onConnect(BLEClient *pClient);
	void onDisconnect(BLEClient *pClient);

    bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params);
    uint32_t onPassKeyRequest();
    void onAuthenticationComplete(ble_gap_conn_desc* desc);
    bool onConfirmPIN(uint32_t pin);
};

class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
  public:
	void onResult(NimBLEAdvertisedDevice *advertisedDevice);
};

#endif