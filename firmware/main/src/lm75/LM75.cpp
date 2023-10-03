#include "LM75.hh"

// The 11 MSB of the Temp sensor are valid, the other 5 bits loaded should be ignored.
#define NUM_TEMP_BITS 11
// The register that contains the 16bit temperature.
#define TEMP_REGISTER 0x00
// The control register
#define CONTROL_REGISTER 0x01
// The uint16_t temperature as an int is divided by this value when converted to a float value.
// Note the data sheet notes this should be 8.0f after throwing away the unused bits
// this is the value with the unused bits set to 0. 
#define TEMP_DIVISOR  256.0f

// 11 bit data from the Temperature sensor
#define BIT_MASK 0x1111111111100000

uint16_t getBitMask(int numBits);

void LM75::enable() {
    bus->beginTransmission(address);
    bus->write(CONTROL_REGISTER);
    bus->write(0x00);
    bus->endTransmission();
    
    delay(100);
}

void LM75::shutdown() {
    bus->beginTransmission(address);
    bus->write(CONTROL_REGISTER);
    bus->write(0x01);
    bus->endTransmission();
}

float LM75::readTemperatureC() {
    bus->beginTransmission(address);
    bus->write(TEMP_REGISTER);
    bus->endTransmission();

    // Read 2 bytes from the register
    bus->requestFrom(address, (uint8_t)2);
    uint16_t tempInt = bus->read() << 8;
    tempInt |= bus->read();
    bus->endTransmission();
    int bits = getBitMask(11);
    tempInt &= getBitMask(NUM_TEMP_BITS);

    return ((float)tempInt) / TEMP_DIVISOR;
}

uint16_t getBitMask(int numBits) {
    u_int8_t oneBits = numBits > 16 ? 16 : numBits;
    uint16_t bitMask = 0;
    for (u_int8_t i = 16 - oneBits; i <= 16; i++) {
        bitMask |= (1 << i);
    }
    return bitMask;
}