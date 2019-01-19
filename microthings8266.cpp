#include <ESP8266WiFi.h>
#include <FS.h>
#include "microthings8266.h"
#include "logonpage.h"




MicroThings8266::MicroThings8266() {
  accessPointMode = false;
  webServer = NULL;
}

void MicroThings8266::setup() {
  SPIFFS.begin();

  if (SPIFFS.exists(SETUP_FILE)) {
    Serial.println("Setup file exists");
    initStationMode();
  } else {
    Serial.println("**Setup file does not exist");
    initAccessPointMode();
  }
  //outputConfig();
}


void MicroThings8266::initStationMode() {
  Serial.println("initStationMode");
  loadStationCfg();
  initWiFi();
}


String MicroThings8266::getClientId() {
  return WiFi.macAddress();
}

void MicroThings8266::loadStationCfg() {
  File f = SPIFFS.open(SETUP_FILE, "r");
  if (f) {
    ssid = f.readStringUntil('\n');
    password = f.readStringUntil('\n');
    //serverIp = f.readStringUntil('\n');
    //serverPort = f.readStringUntil('\n');
    ssid.trim();
    password.trim();
    //serverIp.trim();
    //serverPort.trim();
    Serial.println(ssid);
    Serial.println(password);
    //Serial.println(serverIp);
    //Serial.println(serverPort);
    f.close();
  }
}

//Access Point IP Address http://192.168.4.1/
void MicroThings8266::initAccessPointMode() {
  Serial.println("Init AccessPoint mode");
  accessPointMode = true;

  String mac = AP_SSID + WiFi.macAddress();
  Serial.println(mac);

  WiFi.softAP(mac.c_str(), AP_PWD.c_str());

  webServer = new ESP8266WebServer(80);
  webServer->on("/", HTTP_GET, [this] () {
    handleRoot();
  });
  webServer->on("/save/", HTTP_GET, [this] () {
    handleRootSave();
  });
  webServer->begin();
  Serial.println("HTTP server started");
}

void MicroThings8266::handleRoot() {
  webServer->send(200, "text/html", LOGON_PAGE);
}

void MicroThings8266::handleRootSave() {
  String stationSsid = webServer->arg("ssid");
  String stationPwd = webServer->arg("password");
  Serial.println("stationSsid: " + stationSsid);
  Serial.println("stationPwd: " + stationPwd);
  saveStationCfg(stationSsid, stationPwd);
}

void MicroThings8266::saveStationCfg(String ssid, String pwd) {
  File f = SPIFFS.open(SETUP_FILE, "w");
  if (f) {
    Serial.println("writing setup file to file");
    f.println(ssid);
    f.println(pwd);
    f.close();
  }
}

void MicroThings8266::loop() {
  if (accessPointMode) {
    loopAccessPointMode();
  } else {
    loopStationMode();
  }
}

void MicroThings8266::loopAccessPointMode() {
  webServer->handleClient();
}


void MicroThings8266::initWiFi()
{
  Serial.println("InitWifi");
  WiFi.mode(WIFI_STA);
  Serial.println(ssid);
  Serial.println(password);
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void MicroThings8266::loopStationMode() {
  Serial.println("loopStationMode() - port 80");
  Serial.println("Starting UDP Broadcaser");
  WiFiUDP udp;
  WiFiClient client;

  String ip = WiFi.localIP().toString();
  String multiCastAddress = ip.substring(0, ip.lastIndexOf('.')) + ".255";

  socketServer = new WiFiServer(TCP_PORT);
  socketServer->begin();
  while (!client) {
    //Serial.println("Broadcasting...[" + multiCastAddress + "]");
    udp.beginPacket(multiCastAddress.c_str(), UDP_PORT);
    udp.write(getClientId().c_str());
    udp.endPacket();
    delay(2000);
    client = socketServer->available();
  }

  udp.stopAll();
  socketServer->stop();
  socketServer->close();
  //Client Connected
  Serial.println("Client Connected....");
  uint8_t data[3];
  Serial.println("Sizeof Data:" + sizeof(data));
  client.setNoDelay(true);
  while (client.connected()) {
    if (client.available()) {
      Serial.println("Reading 3 data bytes...");
      int read = client.read(data, 3);
      executeCommand(&client, data);
      yield();
    } else {
      yield();
    }
  }
  Serial.println("***Client Diconnected***");
}

void MicroThings8266::executeCommand(WiFiClient* client, byte* data) {
  Serial.print("Execute Command [");
  Serial.print(data[0]);
  Serial.print(data[1]);
  Serial.print(data[2]);
  Serial.println("]");

  Command cmd = (Command) data[0];
  switch (cmd) {
    case ECHO:
      cmdEcho(client, data);
      break;
    case SETPIN:
      cmdPinMode(client, data);
      break;
    case DIGITAL_READ:
      cmdDigitalRead(client, data);
      break;
    case DIGITAL_WRITE:
      cmdDigitalWrite(client, data);
      break;
    default:
      Serial.println("Invalid Command" + data[0]);
      break;
  }

}

/**
   Echo igores bytes 2 and 3 but expects a NTS to follow. This is sent back
   to the client
*/
void MicroThings8266::cmdEcho(WiFiClient* client, byte* data) {
  Serial.println("Echo");
  String echo = client->readStringUntil('\0');
  Serial.println("Echo String: [" + echo + "]");
  String response = echo + "\n";
  Serial.println("Response String: [" + response + "]");
  client->write(&response.c_str()[0], response.length());
  client->flush();
}

/**
   Pin Mode
   data[1] - pin nbr
   data[2] - 0=INPUT, 1=OUTPUT, 2=Input Pullup
   No Response
*/
void MicroThings8266::cmdPinMode(WiFiClient* client, byte* data) {
  int pin = data[1];
  int pmode = data[2];
  Serial.println("Pin Mode: pin=" + String(pin) + " mode: " + String(pmode));
  pinMode(pin, pmode);
}

/**
   Digital Write
   data[1] - pin nbr
   data[2] - 0=LOW, 1=HIGH
   No Response
*/
void MicroThings8266::cmdDigitalWrite(WiFiClient* client, byte* data) {
  int pin = data[1];
  int state = data[2];
  Serial.println("Digital Write: pin=" + String(pin) + " state: " + String(state));
  digitalWrite(pin, state);
}

/**
   Digital Read
   data[1] - pin nbr
   data[2] - Not used
   Pin state (0 = LOW, 1 = HIGH)
*/
void MicroThings8266::cmdDigitalRead(WiFiClient* client, byte* data) {
  int pin = data[1];
  Serial.println("Digital Read: pin=" + String(pin));
  byte result = digitalRead(pin);
  Serial.println("Pin State:" + String(result));
  client->write((byte*) &result, sizeof(result));
  client->flush();
}
