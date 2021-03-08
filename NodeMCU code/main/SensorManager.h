struct DataPacket{
  PMS::DATA2 pmsData;
  float temperature;
  float humidity;
  int aqi;
};

class SensorManager{
private:
  String DEBUG_TAG = "DATA_READING";
  DHT            * dht22;
  SoftwareSerial * pmsSerial;
  PMS            * pmsReader;

  static const short LOWER_BOUND = 0;
  static const short UPPER_BOUND = 1;

  const short AQI_CHART [7][2]= {
    {  0,  50}, // Good
    { 51, 100}, // Moderate
    {101, 150}, // Unhealthy for senstive group
    {151, 200}, // Unhealthy
    {201, 300}, // Hazardous
    {301, 400}, // Very Hazardous -1
    {401, 500} // Very Hazardous -2
  };

  float PM_CHART_2_5[7][2]= {
    {  0.0,  12.0}, // Good
    { 12.1,  35.4}, // Moderate
    { 35.5,  55.4}, // Unhealthy for senstive group
    { 55.5, 150.4}, // Unhealthy
    {150.5, 250.4}, // Hazardous
    {250.5, 350.4}, // Very Hazardous - 1
    {350.5, 500.4} // Very Hazardous - 1
  };
  void readDHT(DataPacket & readData){
    // Reading DHT22 values
    readData.temperature = dht22->readTemperature();
    readData.humidity = dht22->readHumidity();
    debug(DEBUG_TAG, "DHT22 Temperature", readData.temperature);
    debug(DEBUG_TAG, "DHT22: Humidity", readData.humidity);

    if (isnan(readData.temperature) || isnan(readData.humidity)) {
      debug(DEBUG_TAG, "DHT22: Error! Failed to read data");
    }
  }
  void computeAQI(DataPacket & readData){
    float C =  readData.pmsData.PM_AE_UG_2_5;
    short range = getRange(PM_CHART_2_5, C);
    float I_h = AQI_CHART[range][UPPER_BOUND];
    float I_l = AQI_CHART[range][LOWER_BOUND];
    float C_h = PM_CHART_2_5[range][UPPER_BOUND];
    float C_l = PM_CHART_2_5[range][LOWER_BOUND];

    debug(DEBUG_TAG, "I_h", I_h);
    debug(DEBUG_TAG, "I_l", I_l);
    debug(DEBUG_TAG, "C_h", C_h);
    debug(DEBUG_TAG, "C_l", C_l);
    double I = (I_h - I_l) / (C_h - C_l) * (C - C_l) + I_l;

    readData.aqi = I;
  }
  short getRange(float pollutant[7][2], float val){
    for (short i = 0; i < 7; i++){
      if (pollutant[i][LOWER_BOUND] <= val
         && pollutant[i][UPPER_BOUND] >= val)
        return i;
    }
    return 8;
  }

  void readPMS(DataPacket & readData){
    int readAttempts = 200;
    // Reading PMS3003 values
    while (! pmsReader->read(readData.pmsData)){

      // Sligthly wait and retry
      delay(20);
      readAttempts--;

      // If unable to read data in allocated time . . .
      if (readAttempts <= 0){
        // Place invalid data in struct
        readData.pmsData.PM_AE_UG_1_0 = -1;
        readData.pmsData.PM_AE_UG_2_5 = -1;
        readData.pmsData.PM_AE_UG_10_0 = -1;
        debug(DEBUG_TAG, "FAILED to read from PMS3003");
        return;
      }
    }

    // -- This Blocking call need to be improved
    debug(DEBUG_TAG, "PM 1.0 (ug/m3):", readData.pmsData.PM_AE_UG_1_0);
    debug(DEBUG_TAG, "PM 2.5 (ug/m3):", readData.pmsData.PM_AE_UG_2_5);
    debug(DEBUG_TAG, "PM 10 (ug/m3):", readData.pmsData.PM_AE_UG_10_0);
  }
public:
  void begin(Device & device);
  DataPacket read();
};

void SensorManager::begin(Device & device) {
  dht22 = new DHT(device.DHT_PIN, device.DHT_TYPE);
  dht22->begin();

  pmsSerial = new SoftwareSerial(device.PMS_TX, device.PMS_RX);
  pmsSerial->begin(device.PMS_BAUD);
  pmsReader = new PMS(*pmsSerial);
  debug(DEBUG_TAG, " --Sensors ready-- ");
}


DataPacket SensorManager::read(){
  DataPacket readData;
  readDHT(readData);
  readPMS(readData);
  computeAQI(readData);
  return readData;
}
