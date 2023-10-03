#include "TimeServer.hh"
#include <WiFi.h>
#include <time.h>
#include "../WifiConfig.hh"

const long  gmtOffset_sec = -8*60*60;
const char* ntpServer = "pool.ntp.org";
const int   daylightOffset_sec = 3600;

PtTime loadTime(int gmtOffset_sec);

bool TimeServer::isWifiAvailable() {
    if (password == not_set) {
        Serial.println("Set wifi config in WifiConfig.hh");
        return false;
    }
    return true;
}

void TimeServer::initialize() {
    delay(1000);
    WiFi.disconnect();
    delay(3000);

    Serial.printf("Connecting to %s\n", ssid);
    WiFi.begin(ssid, password);
    delay(1000);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");
}

PtTime TimeServer::getTimeFromWifi() {
    PtTime time; 
    time.year = 0;
    do { 
        delay(500);
        time = loadTime(gmtOffset_sec);
    } while (time.year >= 69);
    return time;
}

void TimeServer::shutdown() {
    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

PtTime loadTime(int gmtOffset_sec) {
    PtTime ptTime;
    tm timeinfo;
    
    getLocalTime(&timeinfo);
    Serial.print("Time from http ");
    Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S\n");
  
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getLocalTime(&timeinfo);
    Serial.print("Got time from server ");
    Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S\n");

    ptTime.year = timeinfo.tm_year % 100;
    ptTime.month = timeinfo.tm_mon + 1;
    ptTime.day = timeinfo.tm_mday;
    ptTime.dayOfWeek = timeinfo.tm_wday;
    ptTime.hours = timeinfo.tm_hour;
    ptTime.minutes = timeinfo.tm_min;
    ptTime.seconds = timeinfo.tm_sec;

    return ptTime;
}