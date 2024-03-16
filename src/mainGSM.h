#ifndef MAIN_GSM_H

#define MAIN_GSM_H

#include <Arduino.h>
#include "SIM800L.h"
#include "SoftwareSerial.h"
String URLstr;
#define SIM800_RST_PIN D6


const char APN[] = "airtelgprs.com";

SIM800L* net;
SoftwareSerial SerialModem(D7, D8);


bool connected = false;
bool disconnected = true;

void setupModule() {
    net = new SIM800L((Stream *)&SerialModem, SIM800_RST_PIN, 200, 512, (Stream *)&Serial);
  //net->enableDebug = true;
    // Wait until the module is ready to accept AT commands
  while(!net->isReady()) {
    Serial.println(F("CONERR"));
    delay(1000);
  }
  Serial.println(F("CONOK"));
  net->setPowerMode(NORMAL);
  // Wait for the GSM signal
  uint8_t signal = net->getSignal();
  while(signal <= 0) {
    Serial.println("SIG:" + signal);
    delay(1000);
    signal = net->getSignal();
  }
  Serial.print(F("SIGOK:"));
  Serial.println(signal);
  delay(500);

  // Wait for operator network registration (national or roaming network)
  NetworkRegistration network = net->getRegistrationStatus();
  while(network != REGISTERED_HOME && network != REGISTERED_ROAMING) {
    delay(1000);
    network = net->getRegistrationStatus();
  }
  Serial.println(F("NTREGOK"));
  delay(1000);

  //etup APN for GPRS configuration
  bool success = net->setupGPRS(APN);
  while(!success) {
    success = net->setupGPRS(APN);
    delay(5000);
  }
  Serial.println(F("NETCFGOK"));
}

void resetHTTP() {
  net->sendCommand_P(AT_CMD_HTTPTERM);
  if(!net->readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
    Serial.println(F("ERRHTTPRESET"));
  }
}

void connectNET() {
  for(uint8_t i = 0; i < 10 && !connected; i++) {
    delay(1000);
    connected = net->connectGPRS();
    disconnected = !connected;
  }
  // Check if connected, if not reset the module and setup the config again
  if(connected) {
    Serial.print(F("NETIP:"));
    Serial.println(net->getIP());
  } else {
    Serial.println(F("NETERR"));
    Serial.println(F("RST"));
    //net->reset();
    //setupModule();
    return;
  }
}

void disconnectNET(){
     // Close GPRS connectivity (5 trials)
  disconnected = net->disconnectGPRS();
  for(uint8_t i = 0; i < 5 && !connected; i++) {
    delay(1000);
    disconnected = net->disconnectGPRS();
  }
  
  if(disconnected) {
    Serial.println(F("NETDIS"));
  } else {
    Serial.println(F("NETSTILLCONN"));
  }
}

void reconnectNET() {
    disconnectNET();
    connectNET();
}

void getNetworkStatus() {
  if(net->isConnectedGPRS()){
    Serial.print(F("GPRSCONOK"));
    //Serial.println(net->getIP());
  } else {
    Serial.println(F("ERRGPRSCON"));
    reconnectNET();
  }
}

void setPower(PowerMode pwr) {
      // Go into low power mode
  Serial.println("PWR:" + net->setPowerMode(pwr));
}

#endif