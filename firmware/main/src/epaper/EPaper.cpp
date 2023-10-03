#include "EPaper.hh"
#include <Arduino.h>
#include <Wire.h>
#include "../PinConfig.h"

void sendCommand(uint8_t command);
void sendCommand(uint8_t command, uint8_t data);
void sendCommand(uint8_t command, uint8_t v1, uint8_t v2);
void sendCommand(uint8_t command, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4);
void waitUntilIdle();
void sendSPIByte(uint8_t data);
void resetEPaper();

pixel_format ep_colors[3] = {{.r=0, .g=0, .b=0, .a=255}, {.r=255, .g=255, .b=255, .a=255}, {.r=255, .g=0, .b=0, .a=255}};

void EPaper::initialize() {
    Serial.println("ePaper initialize");

    pinMode(PIN_EP_BUSY,  INPUT);
    pinMode(PIN_EP_RESET, OUTPUT);
    digitalWrite(PIN_EP_RESET, HIGH);
    pinMode(PIN_EP_DATA_CONTROL, OUTPUT);
    digitalWrite(PIN_EP_DATA_CONTROL, HIGH);
    
    pinMode(PIN_EP_CLK, OUTPUT);
    digitalWrite(PIN_EP_CLK, LOW);
    pinMode(PIN_EP_MOSI, OUTPUT);
    digitalWrite(PIN_EP_MOSI, LOW);
    pinMode(PIN_EP_CS , OUTPUT);
    digitalWrite(PIN_EP_CS , HIGH);

    resetEPaper();  
    Serial.println("ePaper reset");

    // Setup power settings
    //  [ ] [ ] [ ] [BD_EN:0] [ ] [VSR_EN:1] [VS_EN:1] [VG_EN:1]
    //  [ ] [ ] [ ] [VCOM_SLEW:0] [ ] [VGT_LVL[3]:7] 
    //  [ ] [ ] [VDH_LVL[6]:0x3f]
    //  [ ] [ ] [VDL_LVL[6]:0x3f]
    //  [ ] [ ] [VDHR_LVL[6]:]
	sendCommand(0x01, 0x07, 0x07, 0x3a, 0x3a);
    // Power On
	sendCommand(0x04);
	waitUntilIdle();
    Serial.println("ePaper powerOn");
    // Panel setting
    //  [ ] [ ] [REG:0] [KW/R:0] [UD:1] [SHL:1] [SHD_N:1] [RST_N:1]
    sendCommand(0X00, 0x0F);
    // Resolution Setting
    // 0x288 x 0x1e0 => 648x480
	sendCommand(0x61, 0x02, 0x88, 0x01, 0xe0);
    // Dual SPI 0x15
    //  [ ] [ ] [MM_EN:0] [DUSPI_EN:0] [ ] [ ] [ ] [ ]
	//sendCommand(0X15, 0x00);
    // VCom and data interval setting
    //  [BDZ:0] [ ] [BDV[2]:0x1] [ ] [ ] [DDX[2]:0x1]
    //  [ ] [ ] [ ] [ ] [CDI[4]:0x7]
	sendCommand(0X50, 0x11, 0x07);
    // TCON
    // [S2G[4]:0x2] [G2S[4]:0x2]
	sendCommand(0X60, 0x22);

    // Setup the system to use an external temperature sensor.
    // [TSE:1][][][][CAL:0b0000]
    // [ByteNum:01] [Add:000] [Pointer:000] -> 01000000
    sendCommand(0x41, 0x80, 0x40);

    // Start data transmission
	sendCommand(0x10); 
	delay(2);
    Serial.println("ePaper ready");
}

void EPaper::nextImageChannel() {
    sendCommand(0x13);
}

void EPaper::showImage() {
    // Display Refresh
    Serial.println("ePaper refresh");
    sendCommand(0x12);
    waitUntilIdle();
    Serial.println("ePaper display done");
}

void EPaper::shutdown() {
    Serial.println("ePaper shutdown");

    // Sleep
    sendCommand(0x02);
    waitUntilIdle();
    
    // Trigger deep sleep
    sendCommand(0x07, 0xA5);
}

void EPaper::sendByte(uint8_t data) {
    digitalWrite(PIN_EP_DATA_CONTROL, HIGH);
    sendSPIByte(data);
}

void resetEPaper() {
    // pull the RESET pin low for 200ms and then release for 200ms.
    digitalWrite(PIN_EP_RESET, HIGH);
    delay(200);
    digitalWrite(PIN_EP_RESET, LOW);
    delay(200);
    digitalWrite(PIN_EP_RESET, HIGH);
    delay(100);
}

void sendCommand(uint8_t command) {
    digitalWrite(PIN_EP_DATA_CONTROL, LOW);
    sendSPIByte(command);
    digitalWrite(PIN_EP_DATA_CONTROL, HIGH);
}

void sendCommand(uint8_t command, uint8_t data) {
    digitalWrite(PIN_EP_DATA_CONTROL, LOW);
    sendSPIByte(command);
    digitalWrite(PIN_EP_DATA_CONTROL, HIGH);
    sendSPIByte(data);
}

void sendCommand(uint8_t command, uint8_t v1, uint8_t v2) {
    digitalWrite(PIN_EP_DATA_CONTROL, LOW);
    sendSPIByte(command);
    digitalWrite(PIN_EP_DATA_CONTROL, HIGH);
    sendSPIByte(v1);
    sendSPIByte(v2);
}

void sendCommand(uint8_t command, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4) {
    digitalWrite(PIN_EP_DATA_CONTROL, LOW);
    sendSPIByte(command);
    digitalWrite(PIN_EP_DATA_CONTROL, HIGH);
    sendSPIByte(v1);
    sendSPIByte(v2);
    sendSPIByte(v3);
    sendSPIByte(v4);
}

void waitUntilIdle() {
    // Wait until PIN_EP_BUSY goes to 0
    int cnt = 0;
    while (digitalRead(PIN_EP_BUSY) == 1) {
        delay(10);
        cnt++;
    }
}

/**
 * Send a byte of data to the epaper using bit banging.
 */ 
void sendSPIByte(uint8_t data) 
{
    digitalWrite(PIN_EP_CS, LOW);
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(PIN_EP_MOSI, (data & 0x80) != 0); 
        digitalWrite(PIN_EP_CLK, HIGH); 
        data <<= 1;    
        digitalWrite(PIN_EP_CLK, LOW);
    }
    digitalWrite(PIN_EP_CS, HIGH);
}

uint8_t getClosestEpColor(pixel_format px) {
  int ep_color = 0;
  int minError = INT_MAX;
  int re, ge, be, error;
  for (int idx = 0; idx<3; idx++) {
    re = ep_colors[idx].r - px.r;
    ge = ep_colors[idx].g - px.g;
    be = ep_colors[idx].b - px.b;
    error = re*re + ge*ge + be*be;

    if (error < minError) {
      ep_color = idx;
      minError = error;
    }
  }
  return ep_color;
}