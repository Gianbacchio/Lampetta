void checkSchedule()  {
        if (minutes>0 && minutes<300) sleep_mtbf=SLEEPNIGHT_MTBF;
        else sleep_mtbf=SLEEPDAY_MTBF;
        if (!suspended && !skip)  {
                if (performanceWeek[3][day] > performanceWeek[1][day]) duedone = true;
                if (!duedone) {
                        if (cmdStatus != "not connected") {
                                if (minutes> performanceWeek[0][day]) {
                                        if (minutes < performanceWeek[0][day] + performanceWeek[1][day] * 2/60) {
                                                if (cmdStatus != "operating") {
                                                        actions("/clean", "", mainchatid, "scheduler");
                                                        schedule = true;
                                                }
                                        }
                                }

                        }
                }
                if (duedone) {
                        if (schedule) {
                                if  (cmdStatus == "operating")  {
                                        actions("/dock", "", mainchatid, "scheduler");
                                        schedule = false;
                                }
                        }
                }
        }

}

String gethour(uint16_t minut) {
        uint8_t minu1 = int(minut / 60);
        if (minu1 < 10) return "0" + String(minu1);
        else return String(minu1);
}

String getminu(uint16_t minut) {
        uint8_t minu1 = int(minut / 60);
        uint8_t minu2 = (minut - 60 * minu1);
        if (minu2 < 10) return "0" + String(minu2);
        else return String(minu2);
}

void updatetime() {  // update time on internet
        String timereply = gettime();
        if (timereply.length() != 0)  {
                //  Serial.println(timereply);
                minutes = (timereply.substring(17, 19).toInt() + localshift) * 60; //add local shift time
                if (legaltime) minutes += 60; //add legal time if true
                minutes += timereply.substring(20, 22).toInt();
                day = getday(timereply);
                if (day != today) { //set today performances to zero cause a new day came
                        skip = false;
                        today = day;
                        performanceWeek[3][day] = 0;
                        performanceWeek[4][day] = 0;
                        writeDataToEeprom();
                }
        }
        else minutes++;
}

uint8_t getday (String timereply) {
        if (timereply.substring(0, 3).equals("Sun")) return 0;
        if (timereply.substring(0, 3).equals("Mon")) return 1;
        if (timereply.substring(0, 3).equals("Tue")) return 2;
        if (timereply.substring(0, 3).equals("Wed")) return 3;
        if (timereply.substring(0, 3).equals("Thu")) return 4;
        if (timereply.substring(0, 3).equals("Fri")) return 5;
        if (timereply.substring(0, 3).equals("Sat")) return 6;
}

String getdayrev (uint8_t daynum) {
        if (daynum == 0) return "Sun";
        if (daynum == 1) return "Mon";
        if (daynum == 2) return "Tue";
        if (daynum == 3) return "Wed";
        if (daynum == 4) return "Thu";
        if (daynum == 5) return "Fri";
        if (daynum == 6) return "Sat";
}


String gettime()  {
        WiFiClient calen;
        String mess = "";
        long now;
        bool avail;
        IPAddress server(129, 6, 13, 35);
        if (calen.connect(server, 80)) {
                // Serial.println(".... connected to server");
                String a = "";
                char c;
                int ch_count = 0;
                calen.println(
                        String("GET /actualtime.cgi?lzbc=siqm9b HTTP/1.1\r\n") +
                        String("Host: 129.6.13.35\r\n") +
                        "Connection: keep-alive\r\n" +
                        "Upgrade-Insecure-Requests: 1\r\n\r\n");
                now = millis();
                avail = false;
                while (millis() - now < 1500) {
                        while (calen.available()) {
                                char c = calen.read();
                                // swSer.write(c);
                                if (ch_count < 700)  {
                                        mess = mess + c;
                                        ch_count++;
                                }
                                avail = true;
                        }
                        if (avail) {
                                break;
                        }
                }
        }
        for (int i = 5; i < mess.length(); i++)  {
                if (mess.substring(i - 5, i) == "Date:") {
                        mess = mess.substring(i + 1, i + 25);
                        break;
                }
        }
        return mess;
}
