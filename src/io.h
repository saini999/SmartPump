#ifndef IO_H

#define IO_H

#include <Arduino.h>

#define deviceID 1

bool state;

#define ON LOW
#define OFF HIGH

#define powerPin -1
#define phaseOKPin -1
#define phaseAPin -1
#define phaseBPin -1
#define batteryPin -1
#define pumpPin D0
#define pumpPin1 LED_BUILTIN

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
    #define lockPin D1
    void setLock(int state){
        digitalWrite(lockPin, state);
    }
#endif

#ifdef USE_LAMP
    #define lampPin D2
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
    //return digitalRead(powerPin);
    return 1;
}


void setPump(int state){
    digitalWrite(pumpPin1, state);
    digitalWrite(pumpPin, state);
}

void setupIO() {
    pinMode(pumpPin, OUTPUT);
    pinMode(pumpPin1, OUTPUT);
    pinMode(powerPin, INPUT);
    digitalWrite(pumpPin, OFF);
    digitalWrite(pumpPin1, ON);
    #ifdef USE_PH_SEQ
        pinMode(phaseAPin, INPUT);
        pinMode(phaseBPin, INPUT);
    #else 
        pinMode(phaseOKPin, INPUT);
    #endif

    #ifdef USE_LOCK
        pinMode(lockPin, OUTPUT);
        digitalWrite(lockPin, OFF);
    #endif
    #ifdef USE_LAMP
        pinMode(lampPin, OUTPUT);
        digitalWrite(lampPin, OFF);
    #endif
    #ifdef USE_BATTERY
        pinMode(batteryPin, INPUT);
    #endif
}

void processData(String data) {
    if(data.indexOf("\"id\":" + deviceID) != -1 
        && data.indexOf("\"state\":") != -1
        #ifdef USE_LOCK
           && data.indexOf("\"stateLock\":") != -1
        #endif
        #ifdef USE_LAMP
            && data.indexOf("\"stateLamp\":") != -1
        #endif
        ){
      state = data.indexOf("\"state\":1") != -1 ? true : false;
      setPump(state ? ON : OFF);
      Serial.print(F("Turning pump ")); Serial.println(state ? "ON" : "OFF");
        #ifdef USE_LOCK
            bool lockState = data.indexOf("\"stateLock\":1") != -1 ? true : false;
            setLock(lockState ? ON : OFF);
            Serial.print(F("Lock ")); Serial.println(lockState ? "ON" : "OFF");
        #endif
        #ifdef USE_LAMP
            bool lampState = data.indexOf("\"stateLamp\":1") != -1 ? true : false;
            setLamp(lampState ? ON : OFF);
            Serial.print(F("Lamp ")); Serial.println(lampState ? "ON" : "OFF");
        #endif
    } else {
      Serial.println(F("BADDATA"));
    }
}



String genPostData(int battery, int powerState, int phSeq, int devID = deviceID) {
    //{\"device_id\": 1, \"battery\": 33, \"powerState\": 1}
    String str1 = "";
    str1 = "{\"device_id\": ";
    str1 = str1 + devID;
    str1 = str1 + ", \"battery\": ";
    str1 = str1 + battery;
    str1 = str1 + ", \"powerState\": ";
    str1 = str1 + powerState;
    str1 = str1 + ", \"phSeq\": ";
    str1 = str1 + phSeq + "}";
    Serial.println(str1);
    return str1;
}

#endif