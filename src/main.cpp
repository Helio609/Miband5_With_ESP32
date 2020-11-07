#include <Arduino.h>
#include <NimBLEDevice.h>
#include "Callbacks.h"
#include "Config.h"
#include "UUID.h"
#include "Utils.h"

void setup() {
	NimBLEDevice::init("ESP32");
	NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND);
	NimBLEDevice::setPower(ESP_PWR_LVL_P9);

	mibandClient = NimBLEDevice::createClient();
	mibandClient->setClientCallbacks(new ClientCallbacks());

  	BLEScan* pBLEScan = NimBLEDevice::getScan();
  	pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  	pBLEScan->setInterval(1349);
  	pBLEScan->setWindow(449);
  	pBLEScan->setActiveScan(true);
  	pBLEScan->start(10, false);
}

void loop() {
	if(!mibandClient->isConnected()) {
		if(advertisedMiband != nullptr) {
			if(!mibandClient->connect(advertisedMiband)) {
				log_i("尝试重新连接");
				return;
			}
			// 获取并注册认证回调事件
			NimBLERemoteService* authServ = mibandClient->getService(authServUUID);
			NimBLERemoteCharacteristic* authChar = authServ->getCharacteristic(authCharUUID);
			
			authChar->subscribe(true,MibandNotifyCallbacks::authNotifyCallback);
			MibandNotifyCallbacks::authNotifyCallback(authChar,nullptr,0,true);

			// 获取并注册状态回调事件
			NimBLERemoteService* statusServ = mibandClient->getService(statusServUUID);
			NimBLERemoteCharacteristic* statusChar = statusServ->getCharacteristic(statusCharUUID);
			
			statusChar->subscribe(true,MibandNotifyCallbacks::statusNotifyCallback);

			// 获取并注册心率回调事件
			NimBLERemoteService* heartRateServ = mibandClient->getService(heartRateServUUID);
			NimBLERemoteCharacteristic* heartRateChar = heartRateServ->getCharacteristic(heartRateCharUUID);

			heartRateChar->subscribe(true,MibandNotifyCallbacks::heartRateNotifyCallback);

			// 发送信息到手环
			NimBLERemoteService* alertServ = mibandClient->getService(alertServUUID);
			NimBLERemoteCharacteristic* alertChar = alertServ->getCharacteristic(alertCharUUID);
			uint8_t msg[100] = {0};
			int length = 0;
			getMsgPacket("提示","突然发现UTF-8也支持",msg,length);
			// 为什么要使用writeValue(data,length)才会发送出去?
			alertChar->writeValue(msg,length);

			// 注册语音回调
			NimBLERemoteService* voiceControlServ = mibandClient->getService(voiceControlServUUID);
			NimBLERemoteCharacteristic* voiceControlChar1 = voiceControlServ->getCharacteristic(voiceControlChar1UUID);
			NimBLERemoteCharacteristic* voiceControlChar2 = voiceControlServ->getCharacteristic(voiceControlChar2UUID);
			NimBLERemoteCharacteristic* voiceControlChar3 = voiceControlServ->getCharacteristic(voiceControlChar3UUID);
			uint8_t data[] = {0x01,0x20,0x19};
			voiceControlChar2->writeValue(data,3);

			voiceControlChar1->subscribe(true,MibandNotifyCallbacks::voiceControlChar1NotifyCallback);
			voiceControlChar2->subscribe(true,MibandNotifyCallbacks::voiceControlChar2NotifyCallback);
			voiceControlChar3->subscribe(true,MibandNotifyCallbacks::voiceControlChar3NotifyCallback);
		}
	}

	delay(1000);
}