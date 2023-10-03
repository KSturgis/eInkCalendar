#include <Wire.h>
#include "src/image/EInkServer.hh"
#include "src/PinConfig.h"

/* Settings for the status LED */
const int PWMFreq = 5000; /* 5 KHz */
const int PWMChannel = 0;
const int PWMResolution = 10;
const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);

// ServerConfig  useSdCardDetect, useExtOne, useSDCardPower;
ServerConfig RevOne = {true, false, false};
ServerConfig RevTwo = {false, true, false};
ServerConfig RevThree = {true, true, true};
ServerConfig activeConfig = RevTwo;
SDCard sdCard = SDCard(activeConfig.useSdCardDetect, activeConfig.useSDCardPower);
EInkServer server = EInkServer(activeConfig, &sdCard);

void setup()
{  
  Serial.begin(115200);

  // Setup the Status LED
  ledcSetup(PWMChannel, PWMFreq, PWMResolution);
  ledcAttachPin(LEDPin, PWMChannel);
  ledcWrite(PWMChannel, MAX_DUTY_CYCLE / 16);
  
  Serial.println("Booting...");
  server.setup();

  // Initialization is complete
  Serial.print("\r\nOk!\r\n");
  ledcWrite(PWMChannel, 0);
}

void loop()
{
  bool cont = server.loop();

  if (!cont) {
    server.shutdown();
  }
}

