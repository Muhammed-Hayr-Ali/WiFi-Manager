#define BLYNK_PRINT Serial
#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>


WiFiManager wifiManager;
/****************************************************************/

char blynk_token[34] = "";
char device_name[22]  = "Smart Switch";
//flag for saving data
bool shouldSaveConfig = false;

const unsigned long CONNECT_TIMEOUT = 10; // Wait 3 minutes to connect to the real AP before trying to boot the local AP
const unsigned long AP_TIMEOUT = 20; // Wait 3 minutes in the config portal before trying again the original WiFi creds

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


void setup()
{
  Serial.begin(115200);



  /****************************************************************/

  Serial.println("mounting FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, buf.get());
        if (error)
          return;
        strcpy(blynk_token, doc["blynk_token"]);
        strcpy(device_name, doc["device_name"]);
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  /****************************************************************/

  WiFiManagerParameter custom_blynk_token("blynk", "Blynk Token", blynk_token, 32);
  WiFiManagerParameter custom_device_name("Device", "Device Name", device_name, 20);

  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.addParameter(&custom_device_name);

  //reset settings - for testing
  //wifiManager.resetSettings();
  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setConnectTimeout(CONNECT_TIMEOUT);
  wifiManager.setTimeout(AP_TIMEOUT);

  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(device_name)) {
    Serial.println("failed to connect and hit timeout");
    Serial.println("Reboot Your Device");
    ESP.restart();
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  strcpy(blynk_token, custom_blynk_token.getValue());
  strcpy(device_name, custom_device_name.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument doc(1024);
    doc["blynk_token"] = blynk_token;
    doc["device_name"] = device_name;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    serializeJson(doc, Serial);
    serializeJson(doc, configFile);
    configFile.close();
  }

  //end save

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  Blynk.config(blynk_token);
  bool result = Blynk.connect();

  if (result != true) {
    Serial.println("Failed To Connect BLYNK Server");
  } else {
    Serial.println("BLYNK Connected");
    Serial.println(blynk_token);
    Serial.println(device_name);
  }
}
void loop() {
  Blynk.run();
}
