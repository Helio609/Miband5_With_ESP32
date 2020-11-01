#ifndef __CONFIG_H
#define __CONFIG_H
#include <string>
#include <NimBLEDevice.h>

extern NimBLEAdvertisedDevice* advertisedMiband;
extern NimBLEClient* mibandClient;
extern std::string mibandAddress;
extern uint8_t key[16];
#endif