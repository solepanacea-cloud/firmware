#include "BNO086Compass.h"
#include "configuration.h"
#include "graphics/Screen.h"
#include <SparkFun_BNO080_Arduino_Library.h>
#include <Wire.h>

#define BNO086_I2C_ADDRESS 0x4A
#define BNO086_UPDATE_INTERVAL_MS 50

static BNO080 bnoImu;
static bool bnoReady = false;

extern graphics::Screen *screen;

static bool i2cDevicePresent(uint8_t address)
{
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    return (error == 0);
}

void bno086CompassInit()
{
    bool foundAt4A = i2cDevicePresent(0x4A);
    bool foundAt4B = i2cDevicePresent(0x4B);

    uint8_t address = 0;
    if (foundAt4A) {
        address = 0x4A;
    } else if (foundAt4B) {
        address = 0x4B;
    } else {
        LOG_WARN("BNO086: no I2C device found at 0x4A or 0x4B, compass disabled");
        return;
    }

    LOG_INFO("BNO086: found device at 0x%02X, initializing...", address);

    if (bnoImu.begin(address, Wire)) {
        bnoImu.enableRotationVector(BNO086_UPDATE_INTERVAL_MS);
        bnoReady = true;
        LOG_INFO("BNO086: compass ready");
    } else {
        LOG_WARN("BNO086: found at 0x%02X but init failed", address);
    }
}

void bno086CompassUpdate()
{
    if (!bnoReady || !screen)
        return;

    if (bnoImu.dataAvailable()) {
        float yawRad = bnoImu.getYaw();
        float yawDeg = yawRad * (180.0f / 3.14159265f);
        float headingDeg = fmod(90.0f - yawDeg + 360.0f, 360.0f);
        screen->setHeading(headingDeg);
    }
}
