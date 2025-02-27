#include "BufferExecute.h"

#include "Global.h"
#include "SoftUART.h"
#include "items/test.h"
#include "items/vButtonOut.h"
#include "items/vCountDown.h"
#include "items/vImpulsOut.h"
#include "items/vInput.h"
#include "items/vLogging.h"
#include "items/vOutput.h"
#include "items/vPwmOut.h"
#include "items/vSensorAnalog.h"
#include "items/vSensorBme280.h"
#include "items/vSensorBmp280.h"
#include "items/vSensorCcs811.h"
#include "items/vSensorDallas.h"
#include "items/vSensorDht.h"
#include "items/vSensorPzem.h"
#include "items/vSensorUltrasonic.h"
#include "items/vSensorUptime.h"
#include "items/vSensorNode.h"

void loopCmdAdd(const String& cmdStr) {
    if (cmdStr.endsWith(",")) {
        orderBuf += cmdStr;
#ifdef EnableUart
        if (jsonReadBool(configSetupJson, "uart")) {
            if (jsonReadBool(configSetupJson, "uartEvents")) {
                if (myUART) {
                    myUART->print(cmdStr);
                    SerialPrint("I", "<=UART", cmdStr);
                }
            }
        }
#endif
    }
}

void fileCmdExecute(const String& filename) {
    String cmdStr = readFile(filename, 4096);
    csvCmdExecute(cmdStr);
}

void csvCmdExecute(String& cmdStr) {
    cmdStr.replace(";", " ");
    cmdStr += "\r\n";
    cmdStr.replace("\r\n", "\n");
    cmdStr.replace("\r", "\n");
    int count = 0;
    while (cmdStr.length()) {
        String buf = selectToMarker(cmdStr, "\n");

        buf = deleteBeforeDelimiter(buf, " ");  //отсечка чекбокса

        count++;

        if (count > 1) {
            //SerialPrint("I", "Items", buf);
            String order = selectToMarker(buf, " ");  //отсечка самой команды
            if (order == F("button-out")) {
#ifdef EnableButtonOut
                sCmd.addCommand(order.c_str(), buttonOut);
#endif
            } else if (order == F("pwm-out")) {
#ifdef EnablePwmOut
                sCmd.addCommand(order.c_str(), pwmOut);
#endif
            } else if (order == F("button-in")) {
#ifdef EnableButtonIn
                sCmd.addCommand(order.c_str(), buttonIn);
#endif
            } else if (order == F("input")) {
#ifdef EnableInput
                sCmd.addCommand(order.c_str(), input);
#endif
            } else if (order == F("output")) {
#ifdef EnableOutput
                sCmd.addCommand(order.c_str(), output);
#endif
            } else if (order == F("analog-adc")) {
#ifdef EnableSensorAnalog
                sCmd.addCommand(order.c_str(), analogAdc);
#endif
            } else if (order == F("ultrasonic-cm")) {
#ifdef EnableSensorUltrasonic
                sCmd.addCommand(order.c_str(), ultrasonic);
#endif
            } else if (order == F("dallas-temp")) {
#ifdef EnableSensorDallas
                sCmd.addCommand(order.c_str(), dallas);
#endif
            } else if (order == F("dht")) {
#ifdef EnableSensorDht
                sCmd.addCommand(order.c_str(), dhtSensor);
#endif
            } else if (order == F("bme280")) {
#ifdef EnableSensorBme280
                sCmd.addCommand(order.c_str(), bme280Sensor);
#endif
            } else if (order == F("bmp280")) {
#ifdef EnableSensorBmp280
                sCmd.addCommand(order.c_str(), bmp280Sensor);
#endif
            } else if (order == F("ccs811")) {
#ifdef EnableSensorCcs811
                sCmd.addCommand(order.c_str(), ccs811Sensor);
#endif
            } else if (order == F("pzem")) {
#ifdef EnableSensorPzem
                sCmd.addCommand(order.c_str(), pzemSensor);
#endif
            } else if (order == F("uptime")) {
#ifdef EnableSensorUptime
                sCmd.addCommand(order.c_str(), uptimeSensor);
#endif
            } else if (order == F("logging")) {
#ifdef EnableLogging
                sCmd.addCommand(order.c_str(), logging);
#endif
            } else if (order == F("impuls-out")) {
#ifdef EnableImpulsOut
                sCmd.addCommand(order.c_str(), impuls);
#endif
            } else if (order == F("count-down")) {
#ifdef EnableCountDown
                sCmd.addCommand(order.c_str(), countDown);
#endif
            } else if (order == F("impuls-in")) {
#ifdef EnableImpulsIn
                //sCmd.addCommand(order.c_str(), impulsInSensor);
#endif
            } else if (order == F("sensor-node")) {
#ifdef EnableSensorNode
                sCmd.addCommand(order.c_str(), nodeSensor);
#endif
            }

            sCmd.readStr(buf);
        }
        cmdStr = deleteBeforeDelimiter(cmdStr, "\n");
    }
}

void spaceCmdExecute(String& cmdStr) {
    cmdStr += "\r\n";
    cmdStr.replace("\r\n", "\n");
    cmdStr.replace("\r", "\n");
    while (cmdStr.length()) {
        String buf = selectToMarker(cmdStr, "\n");
        sCmd.readStr(buf);
        cmdStr = deleteBeforeDelimiter(cmdStr, "\n");
    }
}

void loopCmdExecute() {
    if (orderBuf.length()) {
        String tmp = selectToMarker(orderBuf, ",");  //выделяем первую команду rel 5 1,
        SerialPrint("I", "CMD", "do: " + tmp);
        sCmd.readStr(tmp);                                //выполняем
        orderBuf = deleteBeforeDelimiter(orderBuf, ",");  //осекаем
    }
}

void addKey(String& key, String& keyNumberTable, int number) {
    keyNumberTable += key + " " + String(number) + ",";
}

int getKeyNum(String& key, String& keyNumberTable) {
    String keyNumberTableBuf = keyNumberTable;

    int number = -1;
    while (keyNumberTableBuf.length()) {
        String tmp = selectToMarker(keyNumberTableBuf, ",");
        String keyIncomming = selectToMarker(tmp, " ");
        if (keyIncomming == key) {
            number = selectToMarkerLast(tmp, " ").toInt();
        }
        keyNumberTableBuf = deleteBeforeDelimiter(keyNumberTableBuf, ",");
    }
    return number;
}

String getValue(String& key) {
    String live = jsonReadStr(configLiveJson, key);
    String store = jsonReadStr(configStoreJson, key);
    if (live != nullptr) {
        return live;
    } else if (store != nullptr) {
        return store;
    } else if (store == nullptr && live == nullptr) {
        return "no value";
    } else {
        return "data error";
    }
}