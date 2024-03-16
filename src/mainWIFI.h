#ifndef MAIN_WIFI_H
#define MAIN_WIFI_H

    #include "io.h"
    #include <ESP8266WiFi.h>
    #include <ESP8266HTTPClient.h>
    #include <WiFiClient.h>
    const char* ssid = "SSID";
    const char* password = "PASS";

    //Your Domain name with URL path or IP address with path
    String serverName;

    // the following variables are unsigned longs because the time, measured in
    // milliseconds, will quickly become a bigger number than can be stored in an int.
    unsigned long lastTime = 0;
    // Timer set to 10 minutes (600000)
    //unsigned long timerDelay = 600000;
    // Set timer to 5 seconds (5000)
    unsigned long timerDelay = 5000;

    void setupWifi() {
        serverName = F("http://api.gaditc.in/iot?device_id=");
        WiFi.begin(ssid, password);
        Serial.println("Connecting.");
        while(WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
        
        Serial.println("");

    }


    void GET() {
        if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;

        //String serverPath = serverName + deviceID;
        
        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName + deviceID);
    
        // If you need Node-RED/server authentication, insert user and password below
        //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
            
        // Send HTTP GET request
        int httpResponseCode = http.GET();
        
        if (httpResponseCode>0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            processData(payload);
            Serial.println(payload);
        }
        else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
            // Free resources
            http.end();
        }
        else {
            Serial.println("WiFi Disconnected");
        }
    }

    void POST() {
        if(WiFi.status()== WL_CONNECTED){
            WiFiClient client;
            HTTPClient http;
            
            http.begin(client, serverName + deviceID);
        
            // If you need Node-RED/server authentication, insert user and password below
            //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
        
            http.addHeader(F("Content-Type"), F("application/json"));
            // Data to send with HTTP POST          
            // Send HTTP POST request
            int httpResponseCode = http.POST(genPostData(getBattery(), getPowerState(), getPhaseSeq()));
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
                
            // Free resources
            http.end();
        }
        else {
            Serial.println("WiFi Disconnected");
        }
  }



#endif