#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#define _USE_LONG_TIME_T

#define PIN_VBAT_AD GPIO_NUM_36
const int LEDPin = GPIO_NUM_2; 
const int SD_CS = GPIO_NUM_15;
const int SD_CD = GPIO_NUM_34;
const int SD_MISO = GPIO_NUM_14;
const int SD_MOSI = GPIO_NUM_13;
const int SD_CLK = GPIO_NUM_27;
const int SD_EN = GPIO_NUM_26;

#define PIN_RTC_INT         GPIO_NUM_25

const int PIN_EP_BUSY = GPIO_NUM_4;
const int PIN_EP_RESET = GPIO_NUM_7;
const int PIN_EP_DATA_CONTROL = GPIO_NUM_8;
const int PIN_EP_CS = GPIO_NUM_5;
const int PIN_EP_CLK = GPIO_NUM_19;
const int PIN_EP_MOSI = GPIO_NUM_20;

#define PIN_I2C_SDA         GPIO_NUM_21
#define PIN_I2C_SCL         GPIO_NUM_22

#endif // PIN_CONFIG_H