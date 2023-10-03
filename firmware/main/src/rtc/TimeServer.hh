#ifndef  TIME_SERVER_H
#define  TIME_SERVER_H
#include "PT7C4337.h"

/**
 * Used when setting the clock time from the WiFi using an NTP server.
*/
class TimeServer {
public:
    bool isWifiAvailable();
    void initialize();
    PtTime getTimeFromWifi();
    void shutdown();
};

#endif //TIME_SERVER_H