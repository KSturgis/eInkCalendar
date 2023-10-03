#ifndef  LM75_H
#define  LM75_H
#include <Arduino.h>
#include <Wire.h>

/**
 * Class for reading Temperature from a generic LM75 style I2C temperature
 * sensor. Only supports reading the temperature
*/
class LM75 {
public:
    LM75(TwoWire *bus, uint8_t address) {
        this->bus = bus;
        this->address = address;
    }

    /**
     * Enable the IC removing it from shutdown mode.
    */
    void enable();

    /**
     * Put the IC into shutdown mode which saves power.
    */
    void shutdown();

    /**
     * Get the current temperature of the sensor 
    **/
    float readTemperatureC();
private:
    uint8_t address;
    TwoWire *bus;
};
#endif //LM75_H