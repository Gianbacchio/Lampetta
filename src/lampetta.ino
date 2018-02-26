/**************************************************************
      LampaBot
      Domestic Lamp remoted using Home Assistance and Telegram
*                                                            *
      written by Giacarlo Bacchio (Gianbacchio on Github)
**************************************************************/


#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManagerData.h>
#include <EEPROM.h>
#include <PubSubClient.h>


#define MQTT_VERSION MQTT_VERSION_3_1_1
#define BUTTON1 0
#define BUTTON2 4
#define BUTTON3 12
#define BUTTON4 14
#define NEOLEDNUM 18  // amount of neopixels available in the strip
#define NEOPIN 5
#define TRIGGER_PIN 0  // trigger input to change to APmode
#define BOT_TOKEN_LENGTH 46
#define BOT_NAME_LENGTH 11
#define BOT_REF_LENGTH 16
#define USER_LENGTH 16
#define PERFORMANCE_LENGTH 2
#define MQTT_MTBF 4e3   //mean time between MQTT check
#define MQTTOFF_MTBF 60e3   //mean time between wifi check
#define WIFI_MTBF 4e3   //mean time between wifi check
#define WAKE_MTBF 5e3  //mean time between wakeUp commands
#define IDLE_MTBF 120e3  //mean time between sleep mode check
#define BOT_MTBF 5e3  //mean time between scan messages
#define TIME_MTBF 60e3  //mean time between calendar time check
#define SLEEPDAY_MTBF 180e3 // mean time between awakeness (daytime)
#define SLEEPNIGHT_MTBF 3600e3 // mean time between awakeness (overnight)
#define WIFIWAKE_MTBF 180e3 //mean time between awakeness
#define HOMEASSIST false  // homeassistant enabled
#define DEEPSLEEP false // deep sleep mode enabled

// Initialize libraries
WiFiManager wifiManager;
WiFiClientSecure client;
WiFiClient client1;
UniversalTelegramBot *bot;  //403422235:AAGcyvamTmoX_Ezsz4ybBcdhld6EuVR_yM4
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOLEDNUM, NEOPIN, NEO_GRB + NEO_KHZ800);


// Initialize Bot data
char botToken[BOT_TOKEN_LENGTH] = "526865500:AAGIHDanh36L4IDY-JBOdGovlNEZRzJ1J0o";
char botName[BOT_NAME_LENGTH] = "Lampetta";
char botRef[BOT_REF_LENGTH] = "Lampetta_bot";
char user1[USER_LENGTH] = "";
char user2[USER_LENGTH] = "";
char user3[USER_LENGTH] = "";
char MQTTser[USER_LENGTH] = "";

// Initialize global variables
int wake_mtbf = WAKE_MTBF * 3; //mean time between wakeUp commands
int idle_mtbf = IDLE_MTBF; //sleep mode after given idle time
int wifi_mtbf = WIFI_MTBF; //mean time between wifi check
int sleep_mtbf = SLEEPDAY_MTBF; //mean time between awakeness
int wifiWake_mtbf= WIFIWAKE_MTBF;  // mean time between wifi wake function
long lastMQTT = 0; //last MQTT check has been done
long lastMQTTOFF = 0; //last MQTT check has been done
long lastTime = 0; //last Calendar check has been done
long lastWake = 0; //last wake up has been done
long lastIdle = 0; //last time entered in idle mode
long lastBot = 0; //last time messages' scan has been done
long lastWifi = 0; //last time wifi check has been done
long lastWifiSleep=0; //last time wifi was in sleep mode
bool modeDeepSleep = false;
bool change = false;
bool shouldSaveConfig = false;
bool apOn = false;
bool wifiLed = false;
bool resetnow = false;
bool wifiMode= false;   // wifi connection enabled
bool modeWifiSleep= false; //wifi sleep mode enabled

String cmdStatus = "not connected";
String mainchatid = "90242404";
String replychatid = "";
uint8_t selectedMode=0; // light mode selected by buttons - default 0 (off)
uint8_t rainbowcolor=0;

// Initialize Calendar variables
uint8_t today = 0; // 0 is sunday, 1 is monday,... 6 is saturday
uint8_t localshift = 1; // Local shift (+1 is Rome)
uint8_t day;
uint16_t performanceWeek[5][7]; // when, hours & meters per day in the week
uint16_t minutes;
bool legaltime = true; // 1 hour shift because of legal time
bool suspended = false; // calendar suspended
bool skip = false; // daily calendar due skipped
bool duedone = false; // daily calendar due completed
bool schedule = false; // cosa fa già??


// Initialize Home Assistant MQTT
// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "office_light1";
const PROGMEM char* MQTT_SERVER_IP = "192.168.1.10";

const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "";
const PROGMEM char* MQTT_PASSWORD = "";

// MQTT: topics
const char* MQTT_LIGHT_STATE_TOPIC = "office/light1/status";
const char* MQTT_LIGHT_COMMAND_TOPIC = "office/light1/switch";

// payloads by default (on/off)
const char* LIGHT_ON = "ON";
const char* LIGHT_OFF = "OFF";
boolean m_light_state = false; // light is turned off by default
PubSubClient MQTTclient(client1);



void setup() {
        Serial.begin(115200);
        pinMode(BUTTON1, INPUT_PULLUP);
        pinMode(BUTTON2, INPUT_PULLUP);
        pinMode(BUTTON3, INPUT_PULLUP);
        pinMode(BUTTON4, INPUT_PULLUP);
        EEPROM.begin(250);
        delay(2000);
        Serial.println("\nLampaBot Power up");
        strip.begin(); // This initializes the NeoPixel library.
        readDataFromEeprom(); // collects all data stored in the EEPROM

        if (!digitalRead(BUTTON2) == LOW && !digitalRead(BUTTON4)== LOW) { // connect request in AP mode
                WiFi.forceSleepWake();
                WiFi.mode(WIFI_STA);
                wifi_station_connect();
                setoff();
                setLed(strip.Color(0, 0, 200), 1); // show SP mode selection active
                configWiFi();
                if (WiFi.status() == WL_CONNECTED)  {
                        wifiMode= true;
                        setLed(strip.Color(0, 0, 125),2); // show AP mode selection before reset
                        Serial.println("\nconnecting bot: " + String(botToken));
                        bot = new UniversalTelegramBot(botToken, client);
                        delay(1000);
                        setoff();
                        Serial.println("\nConnected");
                        updatetime();
                        checkSchedule();
                        setLightMode(0);
                        actions("/message", "wakeup now", mainchatid, "");
                        MQTTclient.setServer(MQTTser, MQTT_SERVER_PORT); // init the MQTT connection
                        MQTTclient.setCallback(callback);
                }
                else  {
                        setLed(strip.Color(200, 0, 0),2); // to inform about lack of connection
                        Serial.println("\nno wifi");
                        delay(2000);
                        setoff();
                }
        }
        else {
                Serial.println("\nNo Wifi mode selected");
                WiFi.mode(WIFI_OFF);
                WiFi.forceSleepBegin();
                delay(1);
                setoff();
                setLed(strip.Color(200, 0, 0), 1); // show SP mode selection active
                delay(2000);
                setoff();
        }
}


void loop() {

        if (digitalRead(BUTTON1) == LOW && digitalRead(BUTTON3)== LOW) { // connect request in AP mode
                Serial.println("\nAP mode request, Reset");
                apOn=true;
                setLed(strip.Color(125, 0, 0),1); // show AP mode selection before reset
                delay(3000);
                resetnow=true;
        }
        if (millis() > lastMQTT + MQTT_MTBF && HOMEASSIST)  {// check MQTT
                if (WiFi.status() == WL_CONNECTED)  {
                      //Serial.println("MQTT Check");
                        if (!MQTTclient.connected()) {
                                if (millis() > lastMQTTOFF + MQTTOFF_MTBF)  {//
                                        if(reconnect()) MQTTclient.loop();
                                        lastMQTTOFF = millis();
                                }
                        }
                        else {MQTTclient.loop();}
                        lastMQTT = millis();
                }
        }
        if ( digitalRead(BUTTON1) == LOW) { // Set light mode 1
                //Serial.println("\nLight mode 1");
                setLightMode(1);
                selectedMode = 1;
        }
        if ( digitalRead(BUTTON2) == LOW) { // Set light mode 2
                //Serial.println("\nLight mode 2");
                selectedMode = 2;
        }
        if ( digitalRead(BUTTON3) == LOW) { // Set light mode 3
                //Serial.println("\nLight mode 3");
                setLightMode(0);
                selectedMode = 0;
        }
        if ( digitalRead(BUTTON4) == LOW) { // Set light mode 4
                //Serial.println("\nLight mode 4");
                selectedMode = 4;
        }
        if (selectedMode == 2 | selectedMode == 4 )
                setLightMode(selectedMode);
        if (resetnow) {
                actions("/message", "reset now", mainchatid, "");
                writeDataToEeprom();
                ESP.reset();
        }
        if (millis() > lastBot + BOT_MTBF && wifiMode)  {// check for new messages on telegram
                if (WiFi.status() == WL_CONNECTED)  {
                        //Serial.println("BOT Check");
                        int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
                        while (numNewMessages) {
                                handleNewMessages(numNewMessages);
                                numNewMessages = bot->getUpdates(bot->last_message_received + 1);
                        }
                }
                lastBot = millis();
                statusLogic();
        }
        if (millis() > lastTime + TIME_MTBF && wifiMode)  {// update time clock
                if (WiFi.status() == WL_CONNECTED)  {
                        //Serial.println("TIME Check");
                        updatetime();
                        checkSchedule();
                }
                lastTime = millis();
        }
        if (millis() > lastIdle + idle_mtbf)  {  //Lampa in sleep mode when idle time out and no operations active
                lastIdle=millis();
                if (cmdStatus != "operating" && !modeWifiSleep) {
                        //actions("/message", String(botName) + " is sleeping", mainchatid, "");
                        modeWifiSleep=true;
                        cmdStatus = "off";
                        Serial.println("\n Wifi sleep");
                        WiFi.mode(WIFI_OFF);
                        WiFi.forceSleepBegin();
                        lastWifiSleep=millis();
                        wifiMode=false;
                        idle_mtbf=10e3; // idle time is reduced for the next wake cycle
                        delay(1);
                }
                if (cmdStatus != "operating" && DEEPSLEEP)
                        modeDeepSleep = true;
        }
        if (modeWifiSleep)  {  // wake wifi
                if (cmdStatus == "operating" ||  millis()> lastWifiSleep+wifiWake_mtbf ) {
                        wakeWifi();
                }
        }
        if (modeDeepSleep) { //Lampa remains sleeping if no commands are coming
                Serial.println("Sleep");
                writeDataToEeprom();
                delay(2000);
                actions("/message", "deep sleep", mainchatid, "");
                deepSleep(sleep_mtbf);
        }
}
