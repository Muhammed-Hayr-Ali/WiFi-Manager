/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}


const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>html {font-family: 'Open Sans', sans-serif;background: #f5f6fa ;color: #444444;}body {display: flex;flex-direction: column;min-height: 93.5vh;margin:0;text-align: center;}header{background-image: linear-gradient(#0f2d56, #0067B8);height: 155px;}header .Logo{margin-top: 15px;display: block;margin-left: auto;margin-right: auto;}.content {width: 90%;padding: 30px 2px 30px 2px;display: block;margin-left: auto;margin-right: auto;}.footer {padding: 5px;margin-top: auto;width: 90%;margin-left: auto;margin-right: auto;border-top: 1px solid #dcdde1;font-size: 11px;color: #7f8fa6;}.mybutton {margin: 10px auto;padding: 15px 25px;text-decoration: none;transition-duration: 0.4s;width: 100%;background:-webkit-gradient(linear, left top, left bottom, color-stop(0.05, #ffffff), color-stop(1, #f6f6f6));border-radius:20px;border:1px solid #dcdcdc;display:inline-block;color: #0f2d56;font-size:15px;font-weight: bold;text-decoration:none;}.q{float: right;width: 64px;text-align: right;}.l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAt0lEQVQ4jaWRQQrCMBREn6VQKT1BF656oB7GWxWXPYUL996hdBUES9GFg/6EpEYcGELmzx9+fiCODhiASRykZaFT0yPglBtyUsMIHMRR2pATMMvcGq2VNucELDKXRiulLaG5yEnMhQNWPktbA1rd2dHsNEVwj8HzxUwVsEuwiqX9hW8BZzGJcqsI3H+ZxvHacr3hqeV5/4J9wlXnEWiAfcBGNev10OP/d4qrvFH0wEUj3gI61bzmJxHMPplBOqqZAAAAAElFTkSuQmCC')no-repeat left center;background-size: 1em;}.Scan a { font-size: 24px;color: #EA2027;}.network {color: #002b4c;text-align: left;}input{outline: none;border-style: none;border-bottom: 1px solid #999;background-color:Transparent;color:#0f2d56;line-height:2.4rem;font-size:1.2rem;width:100%;}::placeholder {color: Gray; opacity: 1;}:-ms-input-placeholder {color: #212121;}::-ms-input-placeholder {color: #212121;}h2 {margin: 10px auto;} a {color: #0067B8;text-decoration: none; font-weight: bold;}.back{margin-top: 20px 0px 0px 10px;text-align: left;}.back a {font-size: 12px;}.MSG {font-size: 19px;color: #0f2d56;margin-top:30px;margin: auto;width:97%;padding: 20px 5px;border-top: 6px solid #0f2d56;border-left: 1px solid #dedede;border-right: 1px solid #dedede;border-bottom: 1px solid #dedede;background:#eaeaea;}#Info { width: 100%;text-align: left;border-collapse: collapse;}#Info td, #Info th { border: 1px solid #ddd; padding: 8px;}#Info th { color:  white;padding: 15px 0px;text-align: center;background: #0f2d56;}th .Name{width: 35%;}th .Value{width: 65%;}#Info td{color: #0f2d56;}#Info tr:nth-child(even){background-color: #eaeaea;}#Info tr{background-color :#fafafa;}.Help {background-color:#0f2d56;;color: #fff;cursor: pointer;padding: 16px;border: none;width:100%;margin:auto;text-align: left;outline: none;font-size: 15px;transition: 0.4s;}.panel {word-wrap: break-word;margin:auto;text-align: left;padding: 0px 10px;background-color: white;max-height: 0;overflow: hidden;transition: max-height 0.4s ease-out;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script> ";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><header><svg class=\"Logo\" width=\"125\" height=\"125\"><svg version=\"1.1\" id=\"Capa_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"  viewBox=\"0 0 26.22 26.22\" style=\"fill:white;enable-background:new 0 0 26.22 26.22;\" xml:space=\"preserve\">   <path d=\"M23.56,20.523c-0.006-1.741-0.002-3.48-0.002-5.222c0-4.55,0-9.103,0-13.654c0-0.943-0.705-1.648-1.641-1.647     c-3.02,0.001-6.036,0-9.054,0C9.938-0.001,7.015,0.001,4.091,0.001c-0.265,0-0.502,0.088-0.716,0.24     C2.841,0.62,2.629,1.143,2.63,1.792C2.638,5.586,2.634,9.38,2.634,13.175c0,3.832,0.001,7.665-0.001,11.495     c0,0.351,0.069,0.67,0.293,0.946c0.342,0.425,0.784,0.604,1.329,0.604c5.834-0.006,11.67-0.004,17.505-0.004     c0.125,0,0.25,0.002,0.375-0.006c0.781-0.049,1.459-0.764,1.455-1.541C23.582,23.285,23.564,21.903,23.56,20.523z M8.405,8.242     c0.135-1.834,0.847-3.423,2.154-4.723c1.145-1.139,2.528-1.854,4.133-2.028c2.881-0.313,5.186,0.735,6.895,3.08     c0.1,0.134,0.102,0.207-0.053,0.306c-0.652,0.419-1.295,0.852-1.961,1.292c-0.937-1.37-2.228-2.114-3.879-2.144     c-1.058-0.019-2.004,0.342-2.855,0.977c-1.611,1.201-2.617,3.871-1.119,6.316c-0.708,0.466-1.416,0.933-2.154,1.419     c-0.1-0.167-0.199-0.321-0.287-0.482C8.603,11.002,8.301,9.659,8.405,8.242z M4.951,15.686c0.652-0.42,1.295-0.854,1.961-1.293     c0.935,1.369,2.226,2.113,3.879,2.143c1.056,0.021,2.005-0.342,2.854-0.977c1.611-1.201,2.617-3.871,1.119-6.316     c0.707-0.467,1.416-0.934,2.154-1.419c0.1,0.167,0.199,0.321,0.285,0.482c0.68,1.253,0.979,2.595,0.879,4.012     c-0.137,1.833-0.85,3.423-2.156,4.724c-1.145,1.14-2.527,1.854-4.132,2.026c-2.88,0.312-5.185-0.733-6.896-3.08     C4.801,15.858,4.798,15.784,4.951,15.686z M7.193,22.663c-0.073,0.063-0.19,0.168-0.19,0.168s-0.156-0.101-0.235-0.146     c-0.332-0.189-0.697-0.17-1.052-0.043c-0.078,0.026-0.156,0.086-0.206,0.151c-0.126,0.17-0.061,0.338,0.147,0.373     c0.271,0.047,0.544,0.078,0.817,0.116c0.055,0.009,0.111,0.015,0.165,0.031c0.488,0.137,0.671,0.599,0.407,1.027     c-0.208,0.344-0.54,0.475-0.917,0.504c-0.366,0.029-0.73-0.018-1.052-0.207c-0.166-0.098-0.271-0.246-0.451-0.396     c0.183-0.106,0.271-0.178,0.388-0.247c0.34,0.362,0.75,0.493,1.229,0.415c0.067-0.012,0.135-0.03,0.197-0.059     c0.161-0.072,0.233-0.203,0.218-0.377c-0.016-0.18-0.162-0.213-0.297-0.234c-0.223-0.034-0.447-0.049-0.671-0.08     c-0.099-0.014-0.197-0.034-0.292-0.062c-0.446-0.146-0.58-0.605-0.285-0.976c0.193-0.242,0.462-0.363,0.757-0.414     c0.368-0.065,0.72-0.056,1.089,0.099c0.132,0.055,0.247,0.149,0.396,0.243C7.272,22.607,7.229,22.631,7.193,22.663z      M11.658,22.396c0,0-0.743,1.459-1.104,2.193l-0.083,0.164H9.932c-0.099-0.584-0.196-1.16-0.302-1.783     c-0.212,0.357-0.4,0.674-0.587,0.988c-0.12,0.205-0.323,0.584-0.432,0.795H8.049c-0.087-0.834-0.172-1.65-0.258-2.479     c0.169,0,0.323,0,0.498,0c0.054,0.553,0.107,1.104,0.162,1.653c0.012,0.005,0.024,0.009,0.036,0.011     c0.182-0.295,0.363-0.59,0.545-0.886c0.121-0.196,0.252-0.389,0.358-0.594l0.122-0.211l0.492,0.002c0,0,0.209,1.123,0.321,1.717     c0.052-0.086,0.088-0.139,0.115-0.194c0.21-0.435,0.424-0.863,0.623-1.304c0.068-0.147,0.102-0.204,0.102-0.204h0.557     L11.658,22.396z M12.256,24.747c-0.164,0-0.318,0-0.502,0c0.174-0.824,0.345-1.64,0.518-2.466c0.161,0,0.32,0,0.504,0     C12.601,23.107,12.429,23.924,12.256,24.747z M15.682,22.676c-0.205,0-0.762,0-0.982,0c-0.088,0.41-0.174,0.812-0.26,1.213     c-0.051,0.238-0.104,0.475-0.147,0.715c-0.021,0.109-0.06,0.166-0.183,0.152c-0.104-0.01-0.211-0.002-0.344-0.002     c0.146-0.697,0.287-1.377,0.436-2.078c-0.33,0-0.64,0-0.966,0c0.03-0.143,0.055-0.264,0.083-0.396c0.806,0,2.438,0,2.438,0     L15.682,22.676z M16.383,24.012c0.062,0.131,0.189,0.254,0.318,0.318c0.424,0.215,0.887,0.086,1.25-0.33     c0.133,0.059,0.271,0.117,0.43,0.186c-0.268,0.439-0.652,0.646-1.139,0.666c-0.187,0.008-0.377-0.004-0.557-0.047     c-0.628-0.145-0.931-0.635-0.854-1.338c0.149-1.373,1.659-1.572,2.36-1.104c0.158,0.105,0.219,0.213,0.381,0.449l-0.398,0.187     c-0.26-0.404-0.635-0.526-1.086-0.429C16.505,22.697,16.122,23.469,16.383,24.012z M21.195,24.749c-0.186,0-0.356,0-0.555,0     c0.088-0.41,0.17-0.81,0.256-1.222c-0.459,0-0.906,0-1.379,0c-0.074,0.347-0.268,1.229-0.268,1.229s-0.389,0-0.516,0     c0.178-0.84,0.35-1.654,0.521-2.479c0.17,0,0.33,0,0.516,0c-0.055,0.274-0.106,0.543-0.164,0.836h1.367     c0,0,0.139-0.603,0.188-0.836c0.189,0,0.363,0,0.552,0C21.541,23.107,21.369,23.924,21.195,24.749z\"/></svg>Sorry, your browser does not support inline SVG.</svg></header> <div class=\"content\">";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button class=\"mybutton\">Configure WiFi</button></form><form action=\"/h\" method=\"get\"><button class=\"mybutton\">Help</button></form><form action=\"/i\" method=\"get\"><button class=\"mybutton\">Info</button></form><form action=\"/r\" method=\"post\"><button class=\"mybutton\">Reboot</button></form>";
const char HTTP_ITEM_START[] PROGMEM      = "<br><br><section class=\"network\">";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#' onclick='c(this)'>{v}</a> <span class=\"q {i}\">{r}%</span></div>";
const char HTTP_SCAN_LINK[] PROGMEM       = "</section><p class=\"Scan text\"><a href=\"/wifi\">Scan</a></p>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><br><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br><input id='{i}' name='{n}' length='{l}' placeholder='{p}' value='{v}' {c}><br/>";
const char HTTP_FORM_END[] PROGMEM        = "<br><button class=\"mybutton\" type='submit'>save</button></form><p class=\"back\"><a href=\"/\">&laquo; back</a></p>";
const char HTTP_SAVED[] PROGMEM           = " <p class=\"MSG\">Try connecting <b>{d}</b> to <b>{x}</b> network.<br>Give it 10 seconds or so <br>If <b>{d}</b> fails to connect to the network <b>{x}</b>, you may have to manually reconnect to <b>{a}</b> network . </p><p class=\"back\"><a href=\"/\">&laquo; back</a></p>";
const char HTTP_REBOOT[] PROGMEM          = "<p class=\"MSG\">  <b>{d}</b><br>will be Rebooting in a few seconds. </p><p class=\"back\"><a href=\"/\">&laquo; back</a></p>";
const char HTTP_INFO[] PROGMEM            = "<h2>Info</h2><table id=\"Info\"> <tr>   <th class=\"Name\">Name</th>   <th class=\"Value\">Value</th> </tr> <tr>";
const char Information[] PROGMEM          = "<br><h2>Information</h2><table id=\"Info\"> <tr>   <th class=\"Name\">Name</th>   <th class=\"Value\">Value</th> </tr> <tr>   <td>Designed by</td><td>XXXXX   </td> </tr><tr>   <td>Email</td><td><a href=\"mailto:mail@mail.com?Subject=Hello\" target=\"_top\">E-mail</a>    </td> </tr>  <tr>   <td>web Site</td><td><a href=\"http://www.yoursite.com/\" target=\"_blank\">Link</a>   </td> </tr><tr>  <td>3rd party</td><td></td> </tr></table><p class=\"back\"><a href=\"/\">&laquo; back</a></p>";
const char HELP[] PROGMEM                 = "<h2>Help</h2><button class=\"Help\">Help 1</button><div class=\"panel\"> <p>   TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT   <br>   TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT  </p></div><button class=\"Help\">Help 2</button><div class=\"panel\"> <p>   TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT   <br>   TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT  </p></div><button class=\"Help\">Help 3</button><div class=\"panel\"> <p>   TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT   <br>   TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT  </p></div><script>var acc = document.getElementsByClassName(\"Help\");var i;for (i = 0; i < acc.length; i++) { acc[i].addEventListener(\"click\", function() {   this.classList.toggle(\"active\");   var panel = this.nextElementSibling;   if (panel.style.maxHeight){     panel.style.maxHeight = null;   } else {     panel.style.maxHeight = panel.scrollHeight + \"px\";   }  });}</script><p class=\"back\"><a href=\"/\">&laquo; back</a></p>";
const char HTTP_END[] PROGMEM             = "</div> <footer class=\"footer\">&copy; P.S.Technology, 2018. All Rights Reserved</footer> </body> </html>";



#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter {
  public:
    /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~WiFiManagerParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();
    ~WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleHelp();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       configPortalHasTimeout();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    int                    _max_params;
    WiFiManagerParameter** _params;

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
