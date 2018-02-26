
void handleNewMessages(int numNewMessages) {
        Serial.print("handleNewMessages: ");
        Serial.println(String(numNewMessages));

        for (int i = 0; i < numNewMessages; i++) {
                String chat_id = String(bot->messages[i].chat_id);
                String text1 = bot->messages[i].text;
                String text = text1.substring(0, text1.indexOf('@'));
                if (text1.substring(text.length() + 1) != "") {
                        if (text1.substring(text.length() + 1) != botRef) break;
                }
                String from_name = bot->messages[i].from_name;
                if (from_name == "") from_name = "Guest";
                if (!text.startsWith("/")) {
                        //    bot->sendMessage(chat_id, "message unknown", "");
                        break;
                }
                String cmd = text.substring(0, text.indexOf(" "));
                String argument = "";
                if (text.length() > cmd.length())
                        argument = text.substring(text.indexOf(" ") + 1);
                Serial.println(cmd+" "+argument+" "+chat_id+" "+from_name);
                actions(cmd, argument, chat_id, from_name);
        }
}


void actions(String cmd, String argument, String chat_id, String from_name) {

        modeDeepSleep = false;
        lastWake = millis();
        lastIdle = millis();
        idle_mtbf=IDLE_MTBF;

        if (cmd == "/message") {
                bot->sendMessage(chat_id, argument, "");
        }

        if (cmd == "/white_full") {
                bot->sendMessage(chat_id, String( botName) + " received command: full_white from " + String(from_name), "");
                setLightMode(1);
                selectedMode = 1;
                change = true;
                replychatid = chat_id;
        }

        if (cmd == "/rainbow") {
                bot->sendMessage(chat_id, String( botName) + " received command: rainbow from " + String(from_name), "");
                selectedMode = 4;
                change = true;
                replychatid = chat_id;
        }

        if (cmd == "/white_dimm") {
                if (argument.length() ==0) {
                        setLightMode(1);
                        selectedMode = 1;
                }
                else      {
                        int colvalue=2.55*argument.toInt();
                        changeColor(strip.Color(colvalue, colvalue, colvalue)); // Red
                }
                bot->sendMessage(chat_id, String( botName) + " received command: dimm light " + argument+ " from "+String(from_name), "");
                change = true;
                replychatid = chat_id;
                cmdStatus = "operating";
        }
        if (cmd == "/reset") {
                resetnow=true;
        }

        if (cmd == "/legaltime") {
                legaltime=!legaltime;
                if (legaltime) bot->sendMessage(chat_id, "legaltime is now active", "");
                else bot->sendMessage(chat_id, "legaltime is now not active", "");
        }

        if (cmd == "/colour") {
                change = true;
                int r = argument.substring(0, argument.indexOf(" ")).toInt();
                argument=argument.substring(argument.indexOf(" ") + 1);
                int g = argument.substring(0, argument.indexOf(" ")).toInt();
                argument=argument.substring(argument.indexOf(" ") + 1);
                int b = argument.substring(0, argument.indexOf(" ")).toInt();
                bot->sendMessage(chat_id, String( botName) + " received command: lit colour " + r+" "+g+" "+b+ " from " + String(from_name), "");
                replychatid = chat_id;
                changeColor(strip.Color(r, g, b));
                cmdStatus = "operating";
                selectedMode = 1;
        }

        if (cmd == "/status") {
                String reply = "";
                reply += "Time now: " + getdayrev(day) + " " + gethour(minutes) + ":" + getminu(minutes) + "\n";
                reply += "Today lighting Report: " + String(performanceWeek[3][day] / 60) + " minutes\n";
                reply += "Schedule status: ";
                if (!suspended) {
                        if (skip) reply += "today skip\n";
                        else reply += "today active\n";
                }
                else reply += "suspended \n";
                reply += "Operative status: "+ cmdStatus;
                bot->sendMessage(chat_id, reply, "");
        }

        if (cmd == "/report") {
                String reply = "";
                reply += "Time now: " + getdayrev(day) + " " + gethour(minutes) + ":" + getminu(minutes) + "\n";
                reply += String( botName) + " Weekly Activity Report\n";
                for (int i = 0; i < 7; i++) {
                        reply += getdayrev(i) + ": " + String(performanceWeek[3][i] / 60) + " minutes";
                }
                bot->sendMessage(chat_id, reply, "");
        }

        if (cmd == "/schedule") {
                String reply = "";
                if (argument.length() == 0) { //list of the stored schedule
                        reply += "Time now: " + getdayrev(day) + " " + gethour(minutes) + ":" + getminu(minutes) + "\n";
                        reply += String( botName) + " schedule is ";
                        if (suspended) reply += "suspended\n";
                        else reply += "active\n";
                        for (int i = 0; i < 7; i++) {
                                reply += getdayrev(i) + ": " + gethour(performanceWeek[0][i]) + ":" + getminu(performanceWeek[0][i]) + " - duration " + String(performanceWeek[1][i] / 60) + "\n";
                        }
                        reply += "\nto set new schedule in minutes, type: \n/schedule d-hh:mm-min  \n(d: Sun = 0...Sat = 6)\n";
                        reply += "/schedule activate - to activate the schedule\n";
                        reply += "/schedule suspend - to suspend the weekly schedule\n";
                        reply += "/schedule skip - to skip the daily work";
                }
                else {
                        if (argument.equals("suspend")) {
                                suspended = true;
                                reply = "schedule is now suspended";
                        }
                        if (argument.equals("activate")) {
                                suspended = false;
                                skip = false;
                                reply = "schedule is now active";
                        }
                        if (argument.equals("skip")) {
                                skip = true;
                                reply = "schedule today is skipped";
                        }
                        if (argument.equals("reset")) {
                                duedone = false;
                                performanceWeek[3][day] = 0;
                                performanceWeek[4][day] = 0;
                                writeDataToEeprom();
                                reply = "schedule today is reset";
                        }
                        if (argument.substring(1, 2).equals("-"))  {
                                int daytoset = argument.substring(0, 1).toInt();
                                performanceWeek[0][daytoset] = 60 * argument.substring(2, 4).toInt() + argument.substring(5, 7).toInt(); // at what time to shedule in minutes
                                performanceWeek[1][daytoset] = argument.substring(8).toInt() * 60; //how many time to work in seconds
                                reply = getdayrev(daytoset) + ": " + gethour(performanceWeek[0][daytoset]) + ": " + getminu(performanceWeek[0][daytoset]) + " - duration " + String(performanceWeek[1][daytoset] / 60) + "\n";
                                writeDataToEeprom();
                        }
                }
                bot->sendMessage(chat_id, reply, "");
        }

        if (cmd == "/off") { // Stop Mode
                setoff();
                replychatid = chat_id;
                bot->sendMessage(chat_id, String( botName) + " received command: Stop Operations from " + from_name, "");
        }

        if (cmd == "/sleep") { // Sleep Mode
                replychatid = chat_id;
                if (cmdStatus != "operating") {
                        setoff();
                        bot->sendMessage(chat_id, String( botName) + " is Power off", "");
                        bot->sendMessage(chat_id, "Now sleep mode", "");
                        idle_mtbf = 0;
                }
                else bot->sendMessage(chat_id, String( botName) + " is operating now, stop the job before Power off", "");
        }

        if (cmd == "/mydata") { //Return the address of the chat
                bot->sendMessage(chat_id, String(chat_id ) + ", " + String(from_name), "");
        }

        String keyboardJson = "[[\"/white_full\", \"/white_dimm 50\",\"/colour\",\"/rainbow\"],[\"/off\",\"/status\",\"/sleep\"],[\"/schedule\",\"/schedule skip\",\"/schedule suspend\",\"/schedule activate\"], [\"/report\",\"/where_r_u\",\"/help\"]]";

        if (cmd == "/start") {
                String welcome = "Wellcome " + from_name + ", this is the control room where you can command " +String(botName)+ "\n";
                welcome += "Choose from one of the following options:";
                bot->sendMessage(chat_id, welcome, "");
                bot->sendMessageWithReplyKeyboard(chat_id, "commands:", "", keyboardJson, true);
        }

        if (cmd == "/options") {
                bot->sendMessage(chat_id, "Choose from one of the following options", "");
                bot->sendMessageWithReplyKeyboard(chat_id, "commands:", "", keyboardJson, true);
        }

        if (cmd == "/help") {
                String welcome = "Hello "+ from_name + ", hereunder are the commands available for your lamp\n";
                welcome += "full_light : to switch the lamp fully white light\n";
                welcome += "white_dimm : to switch the lamp white dimmered\n";
                welcome += "colour : switch the lamp with coloured light\n";
                welcome += "status : to know what the lamp is doing\n";
                welcome += "off : to lit the lamp off\n";
                welcome += "sleep : to command the lamp in sleep mode\n";
                welcome += "schedule : to check or modify weekly program\n";
                welcome += "report : to see a resume of activities along the week\n";
                bot->sendMessage(chat_id, welcome, "Markdown");
        }

}
