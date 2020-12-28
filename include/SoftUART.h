#pragma once

#ifdef uartEnable
#include "Consts.h"
#ifdef ESP8266
#include <SoftwareSerial.h>
extern SoftwareSerial* myUART;
#else
#include <HardwareSerial.h>
extern HardwareSerial* myUART;
#endif

extern void uartInit();
extern void uartHandle();
extern void parse(String& incStr);

#endif