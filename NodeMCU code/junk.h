/*
  This function connect to WiFi network
*/
bool connectWifi()        //Connect to WiFi network
{
  WiFi.begin(wifi_ssid,wifi_password);
  Serial.println("[WIFI STATUS] Connecting To WiFi");
  indicate_mobile_con.set(LOW) ;    // Set WiFi connection indicator to LOW

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("[WIFI CODE] "); Serial.println(WiFi.status());
    indicate_wifi_con.blink(100,50,LOW,0.5);    //Blinking indicates that device is trying to connect to WiFi
    if(WiFi.status()!= WL_DISCONNECTED && WiFi.status() != WL_CONNECTED)
    {
      Serial.println("[WIFI STATUS] Could'nt connect to WiFi");
      indicate_wifi_con.set(LOW);   //Set WiFi connection indicator to Low as device didn't connected to WiFi
      return false;
    }
  }

  Serial.println("[WIFI STATUS] WiFi Connected");
  Serial.print("[IP] ");Serial.println(WiFi.localIP());
  indicate_wifi_con.set(HIGH);  //Set WiFi connection indicator to HIGH as device connected to WiFi
  return true;
}

/*
  This function setups Hotspot of device
*/
void startHotSpot(){

  // Start HoTSpoT so that Moblie can connect to this device
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(HOTSPOT_SSID, HOTSPOT_PASSWORD);
  Serial.println("[HOTSPOT STATUS] Starting");

  // Show IP/Adress of this device.
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("[SOFT_AP IP]");
  Serial.println(myIP);

}
/*
  This function gets a TCP Client(Mobile phone)
  Note:
    The loop inside doesn't break untill connected device does'nt send some data
*/

WiFiClient TCPGetClient()    //Returns TCP client
{
    // Start TCP Server
  server.begin();

  // Wait for client to connect
  Serial.println("[CLIENT STATUS] Waiting for TCPClient");
  while(true){

    client = server.available();
    if (client!=NULL){
      Serial.println("[CLIENT STATUS] TCP CLIENT CONNECTED");
      indicate_mobile_con.set(HIGH);    // After Connecting successfully there is no more blinking and indicator becomes stable
      return client;
    }
    indicate_mobile_con.blink(100,5,LOW,0.5); //Blinking indicates that device is trying to connect to TCP Client
  }
}
/*
  This function send a message to server(url) and return response of server in payload and HTTP code of request
  200 HTTP code is for success
*/
bool SendServerMessage(String url,String message,String& payload, int& httpCode) //Send Http server serialized JSON object and return payload and httpCode
{
       if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status

         HTTPClient http;    //Declare object of class HTTPClient

         http.begin(url);      //Specify request destination
         Serial.print("[SERVER CONNECTION] CONNECTING TO "); Serial.println(url);
         http.addHeader( "Content-Type", "application/json");  //Specify content-type header
         Serial.print("[SERVER CONNECTION] SENDING SERVER MESSAGE : "); Serial.println(message);
         httpCode = http.POST(message);   //Send the request
         payload = http.getString();                  //Get the response payload

         Serial.printf("[HTTP CODE] %d \n",httpCode);   //Print HTTP return code
         Serial.print("[HTTP PAYLOAD] ");Serial.println(payload);    //Print request response payload

         http.end();  //Close connection
         return true;
       }
      else{
        Serial.print("Error in WiFi connection");
        return false;
     }
}
/*
  This function sends Mobile Phone client message
*/
void SendClientMessage(WiFiClient client,String message)
{
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root  = jsonBuffer.createObject();
        root["RES"] = message;
        String json_str;
        root.printTo(json_str);
        client.print(json_str);
}
/*
  This functions send location data to server
*/
String sendLocation()
{
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root  = jsonBuffer.createObject();
        root["device_ID"] = DEVICE_ID;
        root["Longitude"] = longitude;
        root["Latitude"] = latitude;
        String json_str;
        root.printTo(json_str);
        String payload;
        int retCode;
        String retval;
        if(SendServerMessage(URL_LOCATION,json_str,payload,retCode))
        {
          if(retCode==HTTP_CODE_OK)
          {
            StaticJsonBuffer<200> jsonBuffer_2;
            JsonObject& server_response = jsonBuffer_2.parseObject(payload);
            if(!server_response.success())
            {
              Serial.println("parseObject() failed");
            }
            retval=server_response["RES"].asString();
            return retval;
          }
        }

}
/*
  This function Intiats Communication of device with server side
  1.First it starts HotSpot  and let the mobile phone device connect through it to start communication with device
  2.Then it receive data from Mobile phone device keeping that information it tries to connect to WiFi network whose SSID and pasword are provided through phone
  3.If WiFi does't connect it ask device to send data again and it keep doing so untill WiFi connects
  4.As WiFi connects it first send post request to server with Sensor Combination and Email of user
  5.Server Responds by sending DEVICE ID and URL to update Firmware
  6.Then it saves data on EEPROM
  7.Then it send Location of device to server
*/
void InitiateCommunication()
{
      startHotSpot();                         // Turn on HoTSpoT
      while(!WIFI_CONNECTED)                             //Keep asking untill mobile client provide correct WiFi information
      {
          WiFiClient client = TCPGetClient();
          String json = client.readStringUntil('\0');
          Serial.print("[Json String ]");Serial.println(json);

         //====================================== Parsing data received from Mobile Phone ==========================================

          const size_t bufferSize = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(4) + 100;
          DynamicJsonBuffer jsonBuffer(bufferSize);

          JsonObject& root = jsonBuffer.parseObject(json);
          if(!root.success()) {
            Serial.println("parseObject() failed");
          }
          UserEmail = root["Email"];
          wifi_ssid = root["SSID"];
          wifi_password = root["Password"];
          latitude = root["Location"][0];
          longitude = root["Location"][1];

          //====================================== Displaying parsed data ==========================================================

          Serial.print("User Email :");    Serial.println(UserEmail);
          Serial.print("WiFi SSID: ");     Serial.println(wifi_ssid);
          Serial.print("WiFi Password :"); Serial.println(wifi_password);
          Serial.print("Latitude: ");      Serial.println(latitude);
          Serial.print("Longitude: ");     Serial.println(longitude);

          if(connectWifi())
          {
            WIFI_CONNECTED=true;
            Serial.println("WiFi connection successful");
            SendClientMessage(client,"WiFi connection successful");
            break;
          }
          else
          {
            Serial.println("WiFi connection Unsuccessful\nTrying again...");
            SendClientMessage(client,"WiFi connection Unsuccessful");
          }
          client.stop();
      }

      WiFi_SSID = (String)wifi_ssid;
      WiFi_PASS = (String)wifi_password;

      //====================================== Sending Registration data to server ==========================================

      if(WIFI_CONNECTED)
      {

        String payload;
        int retCode;

        String json_str;

        StaticJsonBuffer<200> jsonBuffer_1;
        JsonObject& details_1  = jsonBuffer_1.createObject();

        details_1["email"] = UserEmail;
        details_1["sensor_comb"] = SensorCombination;

        details_1.printTo(json_str);

        Serial.print("[JSON STRING] ");Serial.println(json_str);
        if(SendServerMessage(URL_REGISTRATION,json_str,payload,retCode))
        {
          if(retCode==HTTP_CODE_OK)
          {
            //code for parsing payload goes here
            //it gives device id and url to download firmware
           // Serial.print("[TEST Payload]");Serial.println(payload);
            DynamicJsonBuffer jsonBuffer_2;

            JsonObject& server_response = jsonBuffer_2.parseObject(const_cast<char*>(payload.c_str()));
            if(!server_response.success()) {
              Serial.println("parseObject() failed");
            }
            DEVICE_ID = server_response["device_ID"];

            Serial.printf("[DeviceID Test] %d \n",DEVICE_ID);

            URL_UPDATE = server_response["url_firmware"].as<String>();
          }
        }

        //===================================== Saving credentials in EEPROM ===================================================
        StorageIO rom;
        rom.reposition();
        rom.writeNextString(String(DEVICE_ID));
        rom.writeNextString(WiFi_SSID);
        rom.writeNextString(WiFi_PASS);
        rom.writeNextString(String(longitude));
        rom.writeNextString(String(latitude));


        //===================================== Sending server Location Data ===================================================
        String response=sendLocation();
        Serial.print("[LOCATION RESPONSE] ");Serial.println(response);


      }

}
 /*
  This function updates the firmware of NodeMCU
  NOTE:
  It works fine but as written in the documentation of esp8266 if already existing firmware
  is installed through serial then the device should be reset manually first then OTA update
  will work fine.
 */
void OTA_update_firmware()
{
      indicate_firmware_update.blink(100,20,HIGH,0.7);
      t_httpUpdate_return ret = ESPhttpUpdate.update(URL_UPDATE);
      switch(ret) {
      case HTTP_UPDATE_FAILED:
          Serial.println("[update] Update failed.");
          break;
      case HTTP_UPDATE_NO_UPDATES:
          Serial.println("[update] Update no Update.");
          break;
      case HTTP_UPDATE_OK:
          Serial.println("[update] Update ok."); // may not called we reboot the ESP
          break;
      }
}
