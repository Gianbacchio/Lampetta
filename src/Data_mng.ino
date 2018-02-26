void readDataFromEeprom() {
        uint8_t dataset = EEPROM.read(0);
        apOn = bitRead(dataset, 0);
        modeDeepSleep = bitRead(dataset, 1);
        skip = bitRead(dataset, 2);
        suspended = bitRead(dataset, 3);
        legaltime= bitRead(dataset, 4);
        today = dataset >> 5;
        bitClear(dataset, 0); // no AP mode,
        bitClear(dataset, 1); // no sleep
        EEPROM.write(0, dataset);
        EEPROM.commit();
        int pointer = 1;
        for (int i = 0; i < BOT_TOKEN_LENGTH; i++ ) {
                botToken[i] = EEPROM.read(pointer + i);
        }
        bool namefound=false;
        pointer += BOT_TOKEN_LENGTH;
        for (int i = 0; i < BOT_NAME_LENGTH; i++ ) {
                if (EEPROM.read(pointer + i) == char(208)) {
                        namefound=true;
                        break;
                }
                botName[i] = EEPROM.read(pointer + i);
        }
        if (!namefound) strcpy(botName, "LampaBot");
        Serial.println(botName);
        pointer += BOT_NAME_LENGTH;
        for (int i = 0; i < BOT_REF_LENGTH; i++ ) {
                if (EEPROM.read(pointer + i) == char(208)) break;
                botRef[i] = EEPROM.read(pointer + i);
        }
        pointer += BOT_REF_LENGTH;
        for (int i = 0; i < USER_LENGTH; i++ ) {
                if (EEPROM.read(pointer + i) == char(208)) break;
                user1[i] = EEPROM.read(pointer + i);
        }
        pointer += USER_LENGTH;
        for (int i = 0; i < USER_LENGTH; i++ ) {
                if (EEPROM.read(pointer + i) == char(208)) break;
                user2[i] = EEPROM.read(pointer + i);
        }
        pointer += USER_LENGTH;
        for (int i = 0; i < USER_LENGTH; i++ ) {
                if (EEPROM.read(pointer + i) == char(208)) break;
                user3[i] = EEPROM.read(pointer + i);
        }
        pointer += USER_LENGTH;
        for (int i = 0; i < USER_LENGTH; i++ ) {
                if (EEPROM.read(pointer + i) == char(208)) break;
                MQTTser[i] = EEPROM.read(pointer + i);
        }
        pointer += USER_LENGTH;
        for (int d = 0; d < 7; d++) {
                for (int p = 0; p < 5; p++) {
                        uint8_t data[PERFORMANCE_LENGTH];
                        for (int i = 0; i < PERFORMANCE_LENGTH; i++ ) {
                                data[i] = EEPROM.read(pointer + i);
                        }
                        pointer += PERFORMANCE_LENGTH;
                        performanceWeek[p][d] = turn2x8in1x16(data);
                }
        }
        today = EEPROM.read(pointer + 1);
        pointer += 1;
}

void writeDataToEeprom() {
        int pointer = 0;
        uint8_t dataset = 0;
        dataset = today << 5;
        bitWrite(dataset, 0, apOn);
        bitWrite(dataset, 1, modeDeepSleep);
        bitWrite(dataset, 2, skip);
        bitWrite(dataset, 3, suspended);
        bitWrite(dataset, 4, legaltime);
        EEPROM.write(pointer, dataset);
        pointer++;
        for (int i = 0; i < BOT_TOKEN_LENGTH; i++ ) {
                EEPROM.write(pointer + i, botToken[i]);
        }
        pointer += BOT_TOKEN_LENGTH;
        botName[String(botName).length() + 1] = char(208);
        for (int i = 0; i < BOT_NAME_LENGTH; i++ ) {
                EEPROM.write(pointer + i, botName[i]);
                if (botName[i] == char(208)) break;
        }
        pointer += BOT_NAME_LENGTH;
        botRef[String(botRef).length() + 1] = char(208);
        for (int i = 0; i < BOT_REF_LENGTH; i++ ) {
                EEPROM.write(pointer + i, botRef[i]);
                if (botRef[i] == char(208)) break;
        }
        pointer += BOT_REF_LENGTH;
        user1[String(user1).length() + 1] = char(208);
        for (int i = 0; i < USER_LENGTH; i++ ) {
                EEPROM.write(pointer + i, user1[i]);
                if (user1[i] == char(208)) break;
        }
        pointer += USER_LENGTH;
        user2[String(user2).length() + 1] = char(208);
        for (int i = 0; i < USER_LENGTH; i++ ) {
                EEPROM.write(pointer + i, user2[i]);
                if (user2[i] == char(208)) break;
        }
        pointer += USER_LENGTH;
        user3[String(user3).length() + 1] = char(208);
        for (int i = 0; i < USER_LENGTH; i++ ) {
                EEPROM.write(pointer + i, user3[i]);
                if (user3[i] == char(208)) break;
        }
        pointer += USER_LENGTH;
        MQTTser[String(MQTTser).length() + 1] = char(208);
        for (int i = 0; i < USER_LENGTH; i++ ) {
                EEPROM.write(pointer + i, MQTTser[i]);
                if (MQTTser[i] == char(208)) break;
        }
        pointer += USER_LENGTH;
        for (int d = 0; d < 7; d++) {
                for (int p = 0; p < 5; p++) {
                        uint8_t data[PERFORMANCE_LENGTH];
                        turn1x16in2x8(data, performanceWeek[p][d]);
                        for (int i = 0; i < PERFORMANCE_LENGTH; i++ ) {
                                EEPROM.write(pointer + i, data[i]);
                        }
                        pointer += PERFORMANCE_LENGTH;
                }
        }
        EEPROM.write(pointer + 1, today);
        pointer += 1;

        EEPROM.commit();

        //Serial.println();
        for (int i = 0; i < 250; i++ )  {
                //   Serial.print(char(EEPROM.read(i)));
        }
        // Serial.println();
}

void cleanPerformanceData() { //Clean all performance data of the week
        for (int d = 0; d < 7; d++) {
                for (int p = 0; p < 5; p++) {
                        performanceWeek[p][d] = 0;
                }
        }
}


uint16_t turn2x8in1x16(uint8_t dest[2])  {
        uint16_t decValue = 0;
        String binValue = "";
        for (int w = 0; w < 2; w++) {
                uint8_t bitsCount = 8;
                char str[ bitsCount + 1 ];
                uint8_t i = 0;
                while ( bitsCount-- )
                        str[ i++ ] = bitRead((int) dest[w], bitsCount ) + '0';
                str[ i ] = '\0';
                binValue += str;
        }

        for (int i = 0; i < 16; i++) {
                decValue = pow(2, (15 - i)) * String(binValue.charAt(i)).toInt() + decValue;
        }
        return decValue;
}

void turn1x16in2x8(uint8_t* num2, uint16_t numb)  {
        num2[1] = numb & 255;
        num2[0] = (numb >> 8)  & 255;
}
