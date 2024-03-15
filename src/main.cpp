
#define USE_WIFI

#include <Arduino.h>

#include "io.h"

#ifdef USE_WIFI
#include "mainWIFI.h"
void setup() {
    Serial.begin(9600);
    delay(1000);
    setupWifi();
    setupIO();
    Serial.println("Ready");
    delay(1000);
}

void loop() {
  GET();
  delay(200);
  POST();
  delay(200);
}

#else

#define SERIAL_RX_BUFFER_SIZE 512
#define SERIAL_TX_BUFFER_SIZE 512

#include "mainGSM.h"
#include "GET.h"
#include "POST.h"


void setup() {
    SerialModem.begin(19200);
    Serial.begin(9600);
    delay(1000);
    setupModule();
    delay(1000);
    setPower(NORMAL);
    delay(1000);
    disconnectNET();
    delay(1000);
    connectNET();
    delay(1000);
    Serial.println(F("STARTGET"));
    URLstr = F("http://api.gaditc.in/iot?device_id=");
    URLstr = URLstr + (String)deviceID;
    //Serial.println(URLstr);
    resetHTTP();
    setupIO();
}

void loop() {
  runGET();
  delay(100);
  runPOST();
  delay(100);
}
#endif
