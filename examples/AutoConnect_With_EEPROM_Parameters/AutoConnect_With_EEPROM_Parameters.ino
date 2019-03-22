#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include <BlynkSimpleEsp8266.h>

WiFiManager wifiManager;

/****************************************************************/

//flag for saving data
bool shouldSaveConfig = false;

const unsigned long CONNECT_TIMEOUT = 10; // Wait 10 Seconds  to connect to the real AP before trying to boot the local AP
const unsigned long AP_TIMEOUT = 20; // Wait 20 Seconds in the config portal before trying again the original WiFi creds

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

/********************** Begin EEPROM Section *****************/
#define EEPROM_SALT 12664
typedef struct
{
  int   salt = EEPROM_SALT;
  char blynk_token[33]  = "";
  char device_name[20]  = "";
}

WMSettings;
WMSettings blynk;
WMSettings device;

void eeprom_read()
{
  EEPROM.begin(512);
  EEPROM.get(0, blynk);
  EEPROM.get(100, device);
  EEPROM.end();
}


void eeprom_saveconfig()
{
  EEPROM.begin(512);
  EEPROM.put(0, blynk);
  EEPROM.put(100, device);
  EEPROM.commit();
  EEPROM.end();
}

/*********************************************************************************/

void setup()
{
  Serial.begin(115200);



  /****************************************************************/

  eeprom_read();
  if (blynk.salt != EEPROM_SALT)
  {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    blynk = defaults;
    device = defaults;
  }

  WiFiManagerParameter custom_blynk_token("blynk-token", "Blynk Token", blynk.blynk_token, 33);
  WiFiManagerParameter custom_device_name("device_name", "Device Name",  device.device_name, 20);

  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.addParameter(&custom_device_name);

  //reset settings - for testing
  //wifiManager.resetSettings();
  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality();
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setConnectTimeout(CONNECT_TIMEOUT);
  wifiManager.setTimeout(AP_TIMEOUT);

  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(device.device_name)) {
    Serial.println("failed to connect and hit timeout");
    Serial.println("Reboot Your Device");
    delay(1000);
    ESP.restart();
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  strcpy(blynk.blynk_token, custom_blynk_token.getValue());
  strcpy(device.device_name, custom_device_name.getValue());

  eeprom_saveconfig();

  //end save

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  Blynk.config(blynk.blynk_token);
  bool result = Blynk.connect();

  if (result != true) {
    Serial.println("Failed To Connect BLYNK Server");
  } else {
    Serial.println("BLYNK Connected");
    Serial.println(blynk.blynk_token);
    Serial.println(device.device_name);
  }
}
void loop() {
  Blynk.run();
}
