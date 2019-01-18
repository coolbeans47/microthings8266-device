#ifndef MicroThings8266_h
#define MicroThings8266_h

#include <ESP8266WebServer.h> 
#include <WiFiClient.h>
#include <WiFiUdp.h>

const String SETUP_FILE = "setup.txt";
const String AP_SSID = "MICROTHING:";
const String AP_PWD = "1234567890";
const int UDP_PORT = 7821;
const int TCP_PORT = 7822;

enum Command {ECHO = 0, SETPIN = 1, DIGITAL_READ = 2, DIGITAL_WRITE = 3};

class MicroThings8266
{
  private:
    bool accessPointMode;
    String ssid;
    String password;    
    String description;
    ESP8266WebServer* webServer;
    WiFiServer* socketServer;
    void initStationMode();
    void initAccessPointMode();
    void loopAccessPointMode();
    void loopStationMode();
    void handleRoot();
    void handleRootSave();
    void saveStationCfg(String ssid, String pwd);
    void loadStationCfg();
    String getClientId();
    void initWiFi();
    int countChars(String str, char ch);
    void executeCommand(WiFiClient* client, byte* data);
    void cmdEcho(WiFiClient* client, byte* data);
    void cmdPinMode(WiFiClient* client, byte* data);
    void cmdDigitalWrite(WiFiClient* client, byte* data);
    void cmdDigitalRead(WiFiClient* client, byte* data);
  public:
    MicroThings8266(String desc = "");
    void setup();
    void loop();
};

#endif
