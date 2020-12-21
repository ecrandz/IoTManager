#include "items/SensorPower.h"
#include "SoftUART.h"

PowerSensors myPowerSensors;

void resetPowerSensor() {
    for (auto item : myPowerSensors) {
        item->reset();
    }
}

void readPowerSensor() {
    for (auto item : myPowerSensors) {
        item->loop();
    }
}

void initPowerSensor() {
    if (myUART) {
        myPowerSensors.push_back(new SensorPower(myUART, 0xF8, 60000, "PZEM"));
    }
}

void SensorPower::reset(void) {
    bool res = _pzem->reset();
    SerialPrint("I", "Sensor", "'" + _key + "' reset: " + res);
}

void SensorPower::loop() {
    uint32_t now = millis();
    if ((_lastUpdate + _interval) < now) {
        post("voltage", String(_pzem->values()->voltage, 2));
        post("current", String(_pzem->values()->current, 2));
        post("power", String(_pzem->values()->power, 2));
        post("energy", String(_pzem->values()->energy, 2));
        post("freq", String(_pzem->values()->freq, 0));
        post("pf", String(_pzem->values()->pf, 2));
        _lastUpdate = now;
    }
}

String SensorPower::getDataKey(const char* param_key) {
    String res = _key;
    res += "_";
    res += param_key;
    return res;
}

void SensorPower::post(const char* key, const String& value) {
    String dataKey = getDataKey(key);
    eventGen2(dataKey, value);
    jsonWriteStr(configLiveJson, dataKey, value);
    publishStatus(dataKey, value);
    SerialPrint("I", "Sensor", "'" + dataKey + "' data: " + value);
}
