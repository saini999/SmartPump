#ifndef POST_H

#define POST_H

#include "mainH.h"

//const char POST[] = "http://api.gaditc.in/iot";
const char CONTENT_TYPE[] = "application/json";
//const char PAYLOAD[] = "{\"device_id\": 1, \"battery\": 33, \"powerState\": 1}";


String str1 = "";

void genPostData(int battery, int powerState, int phSeq, int devID = deviceID) {
  //{\"device_id\": 1, \"battery\": 33, \"powerState\": 1}
  str1 = "{\"device_id\": ";
  str1 = str1 + devID;
  str1 = str1 + ", \"battery\": ";
  str1 = str1 + battery;
  str1 = str1 + ", \"powerState\": ";
  str1 = str1 + powerState;
  str1 = str1 + ", \"phSeq\": ";
  str1 = str1 + phSeq + "}";
  Serial.println(str1);
}

void runPOST(){
    char PAYLOAD[str1.length() + 1];
    str1.toCharArray(PAYLOAD, str1.length() + 1);
    Serial.println(PAYLOAD);
    char URL[URLstr.length() + 1];
    URLstr.toCharArray(URL, URLstr.length() + 1);
// Do HTTP POST communication with 10s for the timeout (read and write)
    uint16_t rc = net->doPost(URL, CONTENT_TYPE, PAYLOAD, 10000, 10000);
   if(rc == 200) {
    // Success, output the data received on the serial
    Serial.print(F("POSTOK ("));
    Serial.print(net->getDataSizeReceived());
    Serial.println(F(" b)"));
    Serial.print(F("DT : "));
    Serial.println(net->getDataReceived());
  } else if (rc == 701){
    resetHTTP();
    Serial.print(F("POSTERR"));
    Serial.println(rc);
  } else {
    // Failed...
    Serial.print(F("POSTERR"));
    Serial.println(rc);
  }

}
#endif