# include <EEPROM.h>
# include <StorageIO.h>
#include "ESP8266WiFi.h"

# define DEVICE_ID "1"
# define WIFI_SSID "New Makeistan"
# define WIFI_PASSWORD "makershaker"


StorageIO rom = StorageIO();

void setup(){
}

void loop(){
    rom.writeNextString(DEVICE_ID);
    rom.writeNextString(WIFI_SSID);
    rom.writeNextString(WIFI_PASSWORD);
}
