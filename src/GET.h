#ifndef GET_H

#define GET_H

#include "mainGSM.h"

void runGET() {
  
  char URL[URLstr.length() + 1];
  URLstr.toCharArray(URL, URLstr.length() + 1);
  // Do HTTP GET communication with 10s for the timeout (read)
  uint16_t rc = net->doGet(URL, 10000);
  if(rc == 200) {
    // Success, output the data received on the serial
    Serial.print(F("GETOK ("));
    Serial.print(net->getDataSizeReceived());
    Serial.println(F(" b)"));
    Serial.print(F("DT : "));
    String data = net->getDataReceived();
    Serial.println(data);
    processData(data);
  } else if (rc == 701){
    resetHTTP();
  } else {
    // Failed...
    Serial.print(F("GETERR"));
    Serial.println(rc);
  }

}


#endif