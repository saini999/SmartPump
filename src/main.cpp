#define SERIAL_RX_BUFFER_SIZE 512
#define SERIAL_TX_BUFFER_SIZE 512
#include <Arduino.h>
#include "mainH.h"
#include "io.h"
#include "GET.h"
#include "POST.h"
void setup() {
    Serial1.begin(115200);
    Serial.begin(115200);
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
    Serial.println(URLstr);
    resetHTTP();
    setupIO();
}

void loop() {
  runGET();
  delay(200);
  genPostData(getBattery(), getPowerState(), getPhaseSeq());
  runPOST();
  delay(200);
}
