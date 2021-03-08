class WIFI{
private:
  const char * DEBUG_TAG = "WIFI";
  String ssid;
  String password;

public:
  void begin(Device & device);
  bool getStatus(){ return WiFi.status() == WL_CONNECTED; }

  void setSsid(String name){ ssid = name;}
  void setPassword(String key){ password = key;}
  void setSettings(Device & device);
  void startStation();
};

void WIFI::setSettings(Device & device){
  // Get the sttings from device
  debug(DEBUG_TAG, "Setting up values");
  ssid = device.getSsid();
  password = device.getPassword();
  debug(DEBUG_TAG, "SSID", ssid);
  debug(DEBUG_TAG, "PASSWORD", password);
}

void WIFI::startStation(){
  // Turn of hardware
  debug(DEBUG_TAG, "Turn off hardware", "(STA + SOftAP)");
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  delay(500);
  // Start wifi hardware
  char * name = new char[ssid.length() + 1];
  char * key = new char[password.length() + 1];

  ssid.toCharArray(name, ssid.length() + 1);
  password.toCharArray(key, password.length() + 1);
  WiFi.begin(name, key);

  debug(DEBUG_TAG, "SETTINGS:");
  debug(DEBUG_TAG, "\"" + ssid + "\"", "\"" + password + "\"");

  debug(DEBUG_TAG, "STATION", "started");
  debug(DEBUG_TAG, String("\"") + name + "\"", String("\"") + key + "\"");

  delete name;
  delete key;
}

void WIFI::begin(Device & device) {
  setSettings(device);
  startStation();
}
