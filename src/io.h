#ifndef IO_H

#define IO_H

#include "mainH.h"

#define ON HIGH
#define OFF LOW

#define powerPin -1
#define phaseOKPin -1
#define phaseAPin -1
#define phaseBPin -1
#define batteryPin -1
#define pumpPin LED_BUILTIN

#ifdef USE_PH_SEQ
    unsigned long t1_start, t1_end;

    void measure () {
        while (!digitalRead(phaseAPin)) {
            t1_start = micros();
        }
        while (!digitalRead(phaseBPin)) {
            t1_end = micros();
        }
    }


    int calculations(int g) {
        unsigned int k = 0;
        //To complete number of counts
        g=g+1;
        //To convert  into seconds
        float pf = (float)g / 1000000;
        //To convert seconds into degrees
        pf = pf * 50 * 360; //here frequency = 50hz
        k = pf;
        return k;
    }


    int getPhaseSeq() {
        measure();
        unsigned int angle = calculations(t1_end - t1_start);
        Serial.println("angle:" + angle);
        if(angle < 150 && angle > 80){
            return ON;
        } else {
            return OFF;
        }
    }

#else
    int getPhaseSeq(){
        return digitalRead(phaseOKPin);
    }

#endif

#ifdef USE_LOCK
    #define lockPin -1
    void setLock(int state){
        digitalWrite(lockPin, state);
    }
#endif

#ifdef USE_LAMP
    #define lampPin -1
    void setLamp(int state){
        digitalWrite(lampPin, state);
    }
#endif

#ifdef USE_BATTERY
    float configBatteryCalibration;
    int getBattery() {
        return analogRead(batteryPin) * configBatteryCalibration;
    }
#else 
    int getBattery() {
        return 100;
    }
#endif

int getPowerState() {
    return digitalRead(powerPin);
}


void setPump(int state){
    digitalWrite(pumpPin, state);
}

void setupIO() {
    pinMode(pumpPin, OUTPUT);
    pinMode(powerPin, INPUT);
    #ifdef USE_PH_SEQ
        pinMode(phaseAPin, INPUT);
        pinMode(phaseBPin, INPUT);
    #else 
        pinMode(phaseOKPin, INPUT);
    #endif

    #ifdef USE_LOCK
        pinMode(lockPin, OUTPUT);
    #endif
    #ifdef USE_LAMP
        pinMode(lampPin, OUTPUT);
    #endif
    #ifdef USE_BATTERY
        pinMode(batteryPin, INPUT);
    #endif
}

void processData(String data) {
    if(data.indexOf("\"id\":" + deviceID) != -1 
        && data.indexOf("\"state\":") != -1
        #ifdef USE_LOCK
           && data.indexOf("\"lock\":") != -1
        #endif
        #ifdef USE_LAMP
            && data.indexOf("\"lamp\":") != -1
        #endif
        ){
      state = data.indexOf("\"state\":1") != -1 ? true : false;
      setPump(state ? ON : OFF);
      Serial.print(F("Turning pump ")); Serial.println(state ? "ON" : "OFF");
        #ifdef USE_LOCK
            bool lockState = data.indexOf("\"lock\":1") != -1 ? true : false;
            setLock(lockState ? ON : OFF);
            Serial.print(F("Lock ")); Serial.println(lockState ? "ON" : "OFF");
        #endif
        #ifdef USE_LAMP
            bool lampState = data.indexOf("\"lamp\":1") != -1 ? true : false;
            setLamp(lampState ? ON : OFF);
            Serial.print(F("Lamp ")); Serial.println(lampState ? "ON" : "OFF");
        #endif
    } else {
      Serial.println(F("BADDATA"));
    }
}

#endif