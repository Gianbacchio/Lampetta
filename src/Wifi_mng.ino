
void saveConfigCallback () {  //callback notifying us of the need to save config
        //  Serial.println("Should save config");
        shouldSaveConfig = true;
}

void configWiFi()  {
        wifiManager.setTimeout(30);
        WiFiManagerParameter custom_botToken("botToken", "Bot Token", botToken, BOT_TOKEN_LENGTH);
        WiFiManagerParameter custom_botName("botName", "Bot Name", botName, BOT_NAME_LENGTH);
        WiFiManagerParameter custom_botRef("botRef", "Bot Reference", botRef, BOT_REF_LENGTH);
        WiFiManagerParameter custom_user1("user1", "User 1", user1, USER_LENGTH - 1);
        WiFiManagerParameter custom_user2("user2", "User 2", user2, USER_LENGTH - 1);
        WiFiManagerParameter custom_user3("user3", "User 3", user3, USER_LENGTH - 1);
        WiFiManagerParameter custom_MQTTser("MQTTser", "MQTTServer", MQTTser, USER_LENGTH - 1);

        wifiManager.addParameter(&custom_botToken);
        wifiManager.addParameter(&custom_botName);
        wifiManager.addParameter(&custom_botRef);
        wifiManager.addParameter(&custom_user1);
        wifiManager.addParameter(&custom_user2);
        wifiManager.addParameter(&custom_user3);
        wifiManager.addParameter(&custom_MQTTser);
        wifiManager.setSaveConfigCallback(saveConfigCallback);
        if (!apOn) {
                wifiManager.autoConnect(botName);
        }
        if (apOn) {
                Serial.println("\n AP mode on line");
                apOn = false;
                setoff();
                setLed(strip.Color(0, 200, 0),1); // show AP mode selection active
                if (!wifiManager.startConfigPortal(botName)) { // try to connect AP mode, if not, reset
                        Serial.println("failed to connect and hit timeout");
                        EEPROM.write(0, 0); // no AP mode
                        setoff();     // show AP mode selection finished
                        ESP.reset();
                }
                shouldSaveConfig = true;
        }
        if (shouldSaveConfig) {
                Serial.println("storing data to eeprom");
                if (String(custom_botToken.getValue()).length() != 0) strcpy(botToken, custom_botToken.getValue());
                if (String(custom_botName.getValue()).length() != 0) strcpy(botName, custom_botName.getValue());
                if (String(custom_botRef.getValue()).length() != 0) strcpy(botRef, custom_botRef.getValue());
                if (String(custom_user1.getValue()).length() != 0) strcpy(user1, custom_user1.getValue());
                if (String(custom_user2.getValue()).length() != 0) strcpy(user2, custom_user2.getValue());
                if (String(custom_user3.getValue()).length() != 0) strcpy(user3, custom_user3.getValue());
                if (String(custom_MQTTser.getValue()).length() != 0) strcpy(MQTTser, custom_MQTTser.getValue());
                writeDataToEeprom();
        }
}
