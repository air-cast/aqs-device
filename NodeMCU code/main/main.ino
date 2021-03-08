// ------------------------------// Internal/ External Import
#include <ESP8266WiFi.h>         // Wifi Liberary (Node MCU Core)
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <string.h>
#include <WiFiServer.h>          // TCP/Server    (Node MCU Core)
#include <EEPROM.h>              // Accessing EEPROM (Arduino)
#include <ArduinoJson.h>
#include <StorageIO.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PMS.h>
#include <DHT.h>
#include "debug.h"

#include "Device.h"
#include "SensorManager.h"
#include "Display.h"
#include "WIFI.h"
#include "MobileConnection.h"
#include "ServerConnection.h"

Device device;
Display display(device);
WIFI wifi;
SensorManager sensorManager;
MobileConnection mobileConnection;
ServerConnection serverConnection;


void DELAY_SR(int t){
  for (int i = 0; i < t / 20; i++){
    if (mobileConnection.getRequestStatus()){
      display.phone();
      mobileConnection.connect();
      device.setSsid(mobileConnection.getWifiSsid());
      device.setPassword(mobileConnection.getWifiPassword());
      device.save();
      wifi.begin(device);
    }
    delay(20);
  }
}


void setup() {
  debug("MAIN", "Entered in MAIN");
  display.welcome();
  wifi.begin(device);
  sensorManager.begin(device);
  mobileConnection.begin(device);
  serverConnection.begin(device);
}



void loop() {
  DataPacket data = sensorManager.read();
  display.wifiStatus(wifi.getStatus());
  display.data(data);
  serverConnection.sendData(data);
  DELAY_SR(20000);
}
