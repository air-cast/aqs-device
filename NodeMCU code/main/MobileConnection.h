bool volatile static connectionFlag = false;
void M_ISR(){
  connectionFlag = true;
}

class MobileConnection{
private:
  int btBaud;
  int btRx;
  int btTx;
  int btInput;
  int button;

  String wifiSsid;
  String wifiPassword;
  const char * DEBUG_TAG = "MOBILE_CONNECTION";

  void clearConnectionRequest(){
    connectionFlag = false;
  }
  void turnOnBluetooth(){
    digitalWrite(btInput, HIGH);
  }
  void turnOffBluetooth(){
    digitalWrite(btInput, LOW);
  }
public:
  void begin(Device & device);
  bool getRequestStatus();
  void connect();
  String getWifiSsid() { return wifiSsid; }
  String getWifiPassword() { return wifiPassword; }
};

void MobileConnection::begin(Device & device){

  btBaud  = device.BLUETOOTH_BAUD;
  btRx    = device.BLUETOOTH_RX;
  btTx    = device.BLUETOOTH_TX;
  btInput = device.BLUETOOTH_POWER_PIN;
  button  = device.BLUETOOTH_BUTTON_PIN;

  pinMode(btInput, OUTPUT);   // -- intialize pin
  digitalWrite(btInput, LOW); // -- Turn off bluetooth

  pinMode(button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button),
    M_ISR, RISING);
  clearConnectionRequest();
}

void MobileConnection::connect(){
  debug(DEBUG_TAG, "TURN ON BLUETOOTH");
  turnOnBluetooth();

  SoftwareSerial btSerial = SoftwareSerial(btTx, btRx);
  btSerial.begin(btBaud);

  while(! btSerial.available()) delay(20);
  String st = btSerial.readString();
  st.replace("\n", "");
  st.replace("\r", "");
  debug(DEBUG_TAG, "DATA RECIEVED", st);

  int position = st.indexOf(",");
  wifiSsid = st.substring(0, position);
  wifiPassword = st.substring(position + 1);

  debug(DEBUG_TAG, "WIFI SSID", wifiSsid);
  debug(DEBUG_TAG, "WIFI PASSWORD", wifiPassword);

  turnOffBluetooth();
  debug(DEBUG_TAG, "TURN OFF BLUETOOTH");

  clearConnectionRequest();
  debug(DEBUG_TAG, "CLEAR CONNECTION FLAG");
}

bool MobileConnection::getRequestStatus(){
  return connectionFlag;
}
