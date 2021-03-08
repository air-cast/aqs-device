void debug(String, String);
void debug(String, String, String);
void debug(String, String, float);
String stFahion(String str){
  return "\"" + str + "\"";
}
void debug(String DEBUG_TAG, String DEBUG_MESSAGE){
  Serial.println(DEBUG_TAG + ":>> " + DEBUG_MESSAGE);
}

void debug(String DEBUG_TAG, String DEBUG_MESSAGE, String DEBUG_MESSAGE2){
  Serial.println(DEBUG_TAG + ":>> " + DEBUG_MESSAGE + " " + DEBUG_MESSAGE2);
}

void debug(String DEBUG_TAG, String DEBUG_MESSAGE, float DEBUG_MESSAGE2){
  Serial.println(DEBUG_TAG + ":>> " + DEBUG_MESSAGE + " " + String(DEBUG_MESSAGE2) );
}
