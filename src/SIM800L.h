/********************************************************************************
 * Arduino-SIM800L-driver                                                       *
 * ----------------------                                                       *
 * Arduino driver for GSM/GPRS module SIMCom SIM800L to make HTTP/S connections *
 * with GET and POST methods                                                    *
 * Author: Olivier Staquet                                                      *
 * Last version available on https://github.com/ostaquet/Arduino-SIM800L-driver *
 ********************************************************************************
 * MIT License
 *
 * Copyright (c) 2019 Olivier Staquet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************/
#ifndef _SIM800L_H_
#define _SIM800L_H_

#include <Arduino.h>

#define DEFAULT_TIMEOUT 5000
#define RESET_PIN_NOT_USED -1

enum PowerMode {MINIMUM, NORMAL, POW_UNKNOWN, SLEEP, POW_ERROR};
enum NetworkRegistration {NOT_REGISTERED, REGISTERED_HOME, SEARCHING, DENIED, NET_UNKNOWN, REGISTERED_ROAMING, NET_ERROR};

/**
 * AT commands required (const char in PROGMEM to save memory usage)
 */
const char AT_CMD_BASE[] PROGMEM = "AT";                                      // Basic AT command to check the link
const char AT_CMD_ECHO[] PROGMEM = "ATE1&W";                                  // Set command echo mode

const char AT_CMD_CPIN_TEST[] PROGMEM = "AT+CPIN?";                           // Check SIM card status
const char AT_CMD_CPIN_PIN[] PROGMEM = "AT+CPIN=";                            // Configure PIN code

const char AT_CMD_CSQ[] PROGMEM = "AT+CSQ";                                   // Check the signal strengh
const char AT_CMD_ATI[] PROGMEM = "ATI";                                      // Output version of the module
const char AT_CMD_GMR[] PROGMEM = "AT+GMR";                                   // Output version of the firmware
const char AT_CMD_SIM_CARD[] PROGMEM = "AT+CCID";						                  // Get Sim Card version

const char AT_CMD_CFUN_TEST[] PROGMEM = "AT+CFUN?";                           // Check the current power mode
const char AT_CMD_CFUN0[] PROGMEM = "AT+CFUN=0";                              // Switch minimum power mode
const char AT_CMD_CFUN1[] PROGMEM = "AT+CFUN=1";                              // Switch normal power mode
const char AT_CMD_CFUN4[] PROGMEM = "AT+CFUN=4";                              // Switch sleep power mode

const char AT_CMD_CREG_TEST[] PROGMEM = "AT+CREG?";                           // Check the network registration status
const char AT_CMD_SAPBR_GPRS[] PROGMEM = "AT+SAPBR=3,1,\"Contype\",\"GPRS\""; // Configure the GPRS bearer
const char AT_CMD_SAPBR_APN[] PROGMEM = "AT+SAPBR=3,1,\"APN\",";              // Configure the APN for the GPRS
const char AT_CMD_SAPBR_USER[] PROGMEM = "AT+SAPBR=3,1,\"USER\",";            // Configure the USER for the GPRS (linked to APN)
const char AT_CMD_SAPBR_PWD[] PROGMEM = "AT+SAPBR=3,1,\"PWD\",";              // Configure the PWD for the GPRS (linked to APN)
const char AT_CMD_SAPBR1[] PROGMEM = "AT+SAPBR=1,1";                          // Connect GPRS
const char AT_CMD_SAPBR2[] PROGMEM = "AT+SAPBR=2,1";                          // Check GPRS connection status
const char AT_CMD_SAPBR0[] PROGMEM = "AT+SAPBR=0,1";                          // Disconnect GPRS

const char AT_CMD_HTTPINIT[] PROGMEM = "AT+HTTPINIT";                         // Init HTTP connection
const char AT_CMD_HTTPPARA_CID[] PROGMEM = "AT+HTTPPARA=\"CID\",1";           // Connect HTTP through GPRS bearer
const char AT_CMD_HTTPPARA_URL[] PROGMEM = "AT+HTTPPARA=\"URL\",";            // Define the URL to connect in HTTP
const char AT_CMD_HTTPPARA_USERDATA[] PROGMEM = "AT+HTTPPARA=\"USERDATA\",";  // Define the header(s)
const char AT_CMD_HTTPPARA_CONTENT[] PROGMEM = "AT+HTTPPARA=\"CONTENT\",";    // Define the content type for the HTTP POST
const char AT_CMD_HTTPPARA_REDIR[] PROGMEM = "AT+HTTPPARA=\"REDIR\",1";       // Enable HTTP redirection
const char AT_CMD_HTTPSSL_Y[] PROGMEM = "AT+HTTPSSL=1";                       // Enable SSL for HTTP connection
const char AT_CMD_HTTPSSL_N[] PROGMEM = "AT+HTTPSSL=0";                       // Disable SSL for HTTP connection
const char AT_CMD_HTTPACTION0[] PROGMEM = "AT+HTTPACTION=0";                  // Launch HTTP GET action
const char AT_CMD_HTTPACTION1[] PROGMEM = "AT+HTTPACTION=1";                  // Launch HTTP POST action
const char AT_CMD_HTTPREAD[] PROGMEM = "AT+HTTPREAD";                         // Start reading HTTP return data
const char AT_CMD_HTTPTERM[] PROGMEM = "AT+HTTPTERM";                         // Terminate HTTP connection

const char AT_RSP_OK[] PROGMEM = "OK";                                        // Expected answer OK
const char AT_RSP_DOWNLOAD[] PROGMEM = "DOWNLOAD";                            // Expected answer DOWNLOAD
const char AT_RSP_HTTPREAD[] PROGMEM = "+HTTPREAD: ";                         // Expected answer HTTPREAD
const char AT_RSP_SAPBR[] PROGMEM = "+SAPBR: 1,1";                            // Expected answer SAPBR: 1,1

class SIM800L {
  public:
    // Initialize the driver
    // Parameters:
    //  _stream : Stream opened to the SIM800L module (Software or Hardware, usually 9600 bps)
    //  _pinRst (optional) : pin to the reset of the SIM800L module
    //  _internalBufferSize (optional): size in bytes of the internal buffer to handle general IO with the module
    //                        (including URL and maximum payload to send through POST method)
    //  _recvBufferSize (optional) : size in bytes of the reception buffer (max data to receive from GET or POST)
    //  _debugStream (optional) : Stream opened to the debug console (Software of Hardware)
    SIM800L(Stream* _stream, uint8_t _pinRst = RESET_PIN_NOT_USED, uint16_t _internalBufferSize = 128, uint16_t _recvBufferSize = 256, Stream* _debugStream = NULL);
    ~SIM800L();

    // Force a reset of the module
    void reset();

    // Status functions
    bool isReady();
    uint8_t getSignal();
    PowerMode getPowerMode();
    NetworkRegistration getRegistrationStatus();
    char* getVersion();
    char* getFirmware();
    char* getSimCardNumber();
    char* getSimStatus();
    char* getIP();

    // Troubleshooting functions: enable echo mode
    bool enableEchoMode();

    // Define PIN code to activate SIM card
    bool setPinCode(const char *pin);

    // Define the power mode (for parameter: see PowerMode enum)
    bool setPowerMode(PowerMode powerMode);

    // Enable/disable GPRS
    bool setupGPRS(const char *apn);
    bool setupGPRS(const char *apn, const char *user, const char *password);
    bool connectGPRS();
    bool isConnectedGPRS();
    bool disconnectGPRS();

    // HTTP methods
    uint16_t doGet(const char* url, uint16_t serverReadTimeoutMs);
    uint16_t doGet(const char* url, const char* headers, uint16_t serverReadTimeoutMs);
    uint16_t doPost(const char* url, const char* contentType, const char* payload, uint16_t clientWriteTimeoutMs, uint16_t serverReadTimeoutMs);
    uint16_t doPost(const char* url, const char* headers, const char* contentType, const char* payload, uint16_t clientWriteTimeoutMs, uint16_t serverReadTimeoutMs);

    // Obtain results after HTTP successful connections (size and buffer)
    uint16_t getDataSizeReceived();
    char* getDataReceived();

  public:
    // Send command
    void sendCommand(const char* command);
    // Send comment from PROGMEM
    void sendCommand_P(const char* command);
    // Send command with parameter within quotes (template : command"parameter")
    void sendCommand(const char* command, const char* parameter);
    // Send command with parameter within quotes from PROGMEM (template : command"parameter")
    void sendCommand_P(const char* command, const char* parameter);

    // Read from module (timeout in millisec)
    bool readResponse(uint16_t timeout, uint8_t crlfToWait = 2);
    // Read from module and expect a specific answer defined in PROGMEM (timeout in millisec)
    bool readResponseCheckAnswer_P(uint16_t timeout, const char* expectedAnswer, uint8_t crlfToWait = 2);

    // Purge the serial
    void purgeSerial();

    // Find string in another string
    int16_t strIndex(const char* str, const char* findStr, uint16_t startIdx = 0);

    // Manage internal buffer
    void initInternalBuffer();
    void initRecvBuffer();

    // Manage HTTP/S connection
    uint16_t initiateHTTP(const char* url, const char* headers);
    uint16_t readHTTP(uint16_t serverReadTimeoutMs);

  private:
    // Serial line with SIM800L
    Stream* stream = NULL;

    // Serial console for the debug
    Stream* debugStream = NULL;

    // Details about the circuit: pins
    uint8_t pinReset = 0;

    // Internal memory for the shared buffer
    // Used for all reception of message from the module
    char *internalBuffer;
    uint16_t internalBufferSize = 0;

    // Reception buffer
    char *recvBuffer;
    uint16_t recvBufferSize = 0;
    uint16_t dataSize = 0;

    // Enable debug mode
    bool enableDebug = true;
};

#endif // _SIM800L_H_
