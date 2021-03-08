class Display{
private:
  LiquidCrystal_I2C * lcd;
  String deviceId;
  const char * DEBUG_TAG = "DISPLAY";

  unsigned short rows;
  unsigned short cols;

public:
  Display(Device & device);
  void welcome();
  void phone();
  void wifiStatus(bool);
  void data(DataPacket);
};

Display::Display(Device & device){
  this->rows = device.NUM_ROWS;
  this->cols = device.NUM_COLS;
  deviceId = device.getId();
  debug(DEBUG_TAG, "SET DEVICE ID", stFahion(deviceId));

  lcd = new LiquidCrystal_I2C(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  lcd->begin(rows, cols);
  debug(DEBUG_TAG, "LCD SIZE =", String(rows) + "x" +cols);
}

void Display::welcome(){

  debug(DEBUG_TAG, "DISPLAY WELCOME:");
  debug(DEBUG_TAG, "deviceId", stFahion(deviceId));

  //  lcd->backlight();  // turn on background light
  lcd->clear();      // remove all previous content

  lcd->setCursor(0,0);
  lcd->print("Device: "  + deviceId);
  lcd->setCursor(0,1);
  lcd->print("Starting . . .");

  for (int i = 0; i < 16; i++){
    lcd->setCursor(i -4, 3);
    lcd->print('*');
    delay(200);
  }
}


void Display::phone() {

  lcd->clear();
  lcd->setCursor(5,0);
  lcd->print("Connect ");

  lcd->setCursor(1,1);
  lcd->print("your phone");

  lcd->setCursor(1,2);
  lcd->print("with love");
}


void Display::data(DataPacket data){

  lcd->setCursor(0,1);
  lcd->print("(AQI) ");
  lcd->print(data.aqi);
  lcd->setCursor(-4,2);
  lcd->print("PM2.5: "); lcd->print((int) data.pmsData.PM_AE_UG_2_5);

  lcd->setCursor(-4,3);
  lcd->print("T=");
  lcd->print(data.temperature);
  lcd->print("`C");

  lcd->print(" H=");
  lcd->print((int) data.humidity);
  lcd->print("%");
}

void Display::wifiStatus(bool wifiConnected){
  lcd->clear();
  lcd->setCursor(0,0);

  if (wifiConnected)
    lcd->print("WIFI Connected");
  else lcd->print("WIFI Failed");

}
