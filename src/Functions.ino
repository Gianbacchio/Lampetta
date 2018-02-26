void statusLogic()  {
        if (lit())  {
                if (cmdStatus != "operating") {
                        cmdStatus = "operating";
                        change = true;
                        modeDeepSleep = false;
                }
        }
        else  {
                cmdStatus = "off";
        }
}


bool lit() {  // Check if Lampa is awake and operating
        if (  selectedMode != 0)  {
                performanceWeek[3][day] += wake_mtbf / 1000; //value in sec
                return true;
        }

        else return false;
}

void deepSleep(int sleepTime)  {  //sleep for given seconds
        ESP.deepSleep(sleepTime * 1e3); // 20e6 is 20 seconds
}


void wakeWifi() {  // function to wake wifi mode
  Serial.println("\n Wifi wake");
  WiFi.forceSleepWake();
  WiFi.mode(WIFI_STA);
  wifi_station_connect();
  wifiManager.setTimeout(3);
  wifiManager.autoConnect(botName);
  wifiMode=true;
  modeWifiSleep=false;
  if (cmdStatus=="operating") idle_mtbf=IDLE_MTBF;
  lastIdle=millis();
  Serial.println(idle_mtbf);
}
