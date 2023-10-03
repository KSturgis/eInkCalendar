# eInk Calendar
This novel device is a battery powered daily calendar that shows calendar
images daily using a tri color white, black, and read eInk display.

The code is designed to be run on an ESP32 running the Arduino Framework.
Images to be displayed are stored on an SD card that is mounted to the board.

## Operation
In nominal operation on startup the system loads the current time from a Real
Time Clock IC on the circuit board. Using the current time the system will 
find an image to display. For example on "Mar-4-2023" the system may display
an image from the SD card named "\20230304.png". The system will update the
image every day at midnight to the image for the next day.

The system will search for images with the following naming patterns.

"YYYYMMDD.png" -> "MMDD.png" -> "DD.png"

For example if the date is "Dec-25-2034", the system will first look for 
"\20341225.png" if this doesn't exist then the system will look for an 
image name "1225.png", if this isn't present the system will finally
look for an image named "25.png".

The device runs on 2 AAA batteries and will run for months before the 
batteries are drained.

## Supported Images
Images MUST be PNG of size 648x480 and SHOULD only contain the colors Black,
White, and Red.

The system will decode the PNG image, and display the nearest supported color
for each pixel as either black, white, or red. You can pass in any image but
the nearest color support will likely need images that are designed to be 
displayed on a screen supporting these colors.

## OOBE
The device has an OOBE (Out of Box Experience) mode that can be used for the 
initial configuration of the device. An initial image is shown rather than 
the calendar image when in OOBE mode.

> To enter OOBE create an image named "/first_display.png"

Once the oobe image is displayed it is removed from the device. Next time the
device boots it will be in the normal operation mode.

When first created the RTC will not have the current time. During OOBE there
are two options for setting the RTC.

### Setting the clock
There are two methods for setting the clock on a new board. Once set the clock
should should keep time for a long time. The coin cell battery will keep time
running in the RTC for ~4 years without any AAA batteries installed. With
fresh AAA batteries continually in the device will keep time indefinitely.

#### Using WiFi
The easiest way to set the real time clock is to use the WiFi on the ESP32. Wifi 
is disabled during normal operation. During normal operation the WiFi system will 
never power on.

To set the clock do the following;
1. Write OOBE image to the SD card to put the device in OOBE mode 
   "/first_display.png".
2. Create a file "wifi.txt" on the SD card. The file should contain 4 lines
    - [line 1] SSID
    - [line 2] Password
    - [line 3] Time offset from GMT in hours 
    - [line 4] Daylight savings time offset in seconds.

```
YourSSID\r\n
Secret Password!\r\n
-7.0\r\n
3600\r\n
```

If in daylight savings time the last line should be 3600, if not 0.

Note the clock does not respond to daylight savings time, so if set during
daylight savings time the update will happen at 1am when not in daylight 
savings time.

After the OOBE image is displayed the system will connect to the WiFi listed
and then communicate with an NTP server to get the exact current time. Once
the time is determined the system will save the time to the RTC.

Once complete the wifi.txt file will be deleted and further operation will
skip setting the clock. This also means you can distribute the device with
sharing the WiFi credentials used in configuration.

#### Using a time file
Create a file "\time.txt"

```
year,month,day,day_of_week,hours,minutes,seconds\r\n
```

year can be 2023 or 23
day_of_week 1-7, 1: Sunday, 7:Saturday, but this isn't used.
hours 0-23: Always use 24 hour format.

If the "\time.txt" file is present at startup the clock in the RTC will be set
before the image is loaded. After used the file time.txt will be removed so
further boots wont reset the time.

## Low Battery
> On low battery condition the image "/low_power.png" is shown.

## Open source
This uses [PNGdec](https://github.com/bitbank2/PNGdec) to decode PNG images on the
device.

# Repository Structure
/firmware/main/ - Firmware code for ESP32 to run the calendar. This is Arduino code 
  project that builds the firmware that can be flashed to the board.
/hardware/ - Schematic and board designs for the hardware.
/util/ - Scripts and utilities to help generate images to use on the calendar.

# Hardware
In rev1.3 the Esp32 in in the form of a LilyGo T-Micro32 plus that is added to the
board. The system runs off 3V in the form of 2 AA batteries that must be wired to 
the board. The board has a small coin cell battery that keeps an internal real time
clock running when no batteries are in place.

The board includes a micro SD card slot for holding the images to be displayed. This
allows adding almost infinite number of images to the caledar and allows a calendar 
to be updated every year very easily.

A real time clock with battery backup is included so even if batteries are not attached
the device will keep the current time. The system should keep the current time for ~4 years
using the backup battery. The backup battery is a CR1220.

Finally the board has a USB-C connector. This is used to program the firmware and to
read serial debug messages. The USB connector cannot be used to power the device without
batteries.

The board has an temperature sensor on it, that isn't currently useful and will be removed
in future revisions.

Prototypes of the board have all been created and assembled by JLCPCB. The BOM includes
part numbers for ordering assembled boards from them.

# eInk Screen
This device is designed to be used with the [GDEW0583Z83](https://www.good-display.com/product/242.html) 
from Good Display. This is also available from [WaveShare](https://www.waveshare.com/product/displays/e-paper/epaper-1/5.83inch-e-paper-b.htm).
The display can display 3 colors, White, Black, and Red and is used to display
the daily images.




