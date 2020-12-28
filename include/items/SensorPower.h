#pragma once
#include "Global.h"

#include <PZEMSensor.h>

class SensorPower {
   public:
    SensorPower(Stream* serial, uint32_t addr, uint32_t interval, String key) : _lastUpdate{0},
                                                                                        _interval{interval},
                                                                                        _key{key} {
        _pzem = new PZEMSensor(serial, addr);
    }

    ~SensorPower() {
        delete _pzem;
    }

    void loop(void);
    void reset(void);

   private:
    void readSensor(void);
    String getDataKey(const char* param_key);
    void post(const char* key, const String& value);

   private:
    PZEMSensor* _pzem;
    uint32_t _lastUpdate;
    uint32_t _interval;
    String _key;
};

typedef std::vector<SensorPower*> PowerSensors;

extern PowerSensors myPowerSensors;

extern void resetPowerSensor();

extern void initPowerSensor();

extern void readPowerSensor();