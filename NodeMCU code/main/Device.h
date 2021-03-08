class Device{

public:
  //---------------- Display
  //---------------- Size of LCD
  const static int NUM_ROWS = 4;
  const static int NUM_COLS = 16;

  //----------------- DEVICE META
  //----------------- Sensors info
  const char * SENSOR_COMBINATION = "1";
  const char * CODE_VERSION = "1";

  //------------------ BLUE-TOOTH veriables
  const static int BLUETOOTH_BAUD = 9600;
  // communication
  const static int BLUETOOTH_RX = D0;
  const static int BLUETOOTH_TX = D4;
  // switching
  const static int BLUETOOTH_BUTTON_PIN = D7;
  const static int BLUETOOTH_POWER_PIN = D8;

  // PMS-3003 Sensor pins
  const static int PMS_RX = D3;
  const static int PMS_TX = D5;
  const static int PMS_BAUD = 9600;
  // DHT-22 Sensor pins
  const static int DHT_PIN = D6;
  const static int DHT_TYPE = DHT22;

  // LCD pins
  // SDA
  // SCL

  /**********************************************
  *************** Functions will start **********
  ***********************************************/
  Device();
  String getId(){ return id;}
  String getSsid(){ return ssid;}
  String getPassword(){ return password;}
  void setSsid(String ssid);
  void setPassword(String password);
  void save();
private:
  String id;
  String ssid;
  String password;
  const char * DEBUG_TAG = "DEVICE";
};

void Device::setSsid(String ssid){
  this->ssid = ssid;
}
void Device::setPassword(String password){
  this->password = password;
}

Device::Device(){
  Serial.begin(9600);
  delay(500);

  Serial.println();
  Serial.println("***********************");
  Serial.println("_____      ______ _____");
  Serial.println("|    \\       |    |   |");
  Serial.println("|     \\      |    |___|");
  Serial.println("|______\\     |    |\\");
  Serial.println("|       \\    |    | \\");
  Serial.println("|        \\___|___ |  \\");
  Serial.println("CAST: An dream of clean air");

  debug(DEBUG_TAG, "Loading data from EEPROM");

  StorageIO rom;
  rom.reposition();

  id = rom.readNextString();
  ssid = rom.readNextString();
  password = rom.readNextString();

  debug(DEBUG_TAG, "ID", id);
  debug(DEBUG_TAG, "SSID", ssid);
  debug(DEBUG_TAG, "PASSWORD", password);
  debug(DEBUG_TAG, "DONE LOADING");
}

void Device::save(){
  debug(DEBUG_TAG, "Loading data from EEPROM");

  StorageIO rom;
  rom.reposition();

  rom.writeNextString(id);
  rom.writeNextString(ssid);
  rom.writeNextString(password);

  debug(DEBUG_TAG, "ID", id);
  debug(DEBUG_TAG, "SSID", ssid);
  debug(DEBUG_TAG, "PASSWORD", password);
}
