#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "PMS.h"

PMS pms(Serial2);
PMS::DATA data;

void setup(){

  Serial.begin(9600);   // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial2.begin(9600);  // GPIO2 (D4 pin on ESP-12E Development Board)

  WiFi.begin("CYBR_node", "cybr.node1");

  Serial.println();
  Serial.print("CONNECTING to WIFI: ");
  while (! WiFi.isConnected()){
    Serial.print(".");
    delay(500);
  }

  Serial.println("IP: ");
  Serial.print(WiFi.localIP().toString());
}

void loop(){

  Serial.println("Wait max. 1 second for read...");
  if (pms.readUntil(data)){
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);

    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);

    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);

    HTTPClient client(String("http://hello.com"));
  } else {
    Serial.println("No data.");
  }
  delay(1000 * 60);
}
