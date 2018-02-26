
// function called to publish the state of the light (on/off)
void publishLightState() {
        if (m_light_state) {
                MQTTclient.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_ON, true);
        } else {
                MQTTclient.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_OFF, true);
        }
}

// function called to turn on/off the light
void setLightState() {
        if (m_light_state) {
                selectedMode = 4;
                Serial.println("INFO: Turn light on...");
        } else {
                setoff();
                Serial.println("INFO: Turn light off...");
        }
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
        // concat the payload into a string
        String payload;
        for (uint8_t i = 0; i < p_length; i++) {
                payload.concat((char)p_payload[i]);
        }

        // handle message topic
        if (String(MQTT_LIGHT_COMMAND_TOPIC).equals(p_topic)) {
                // test if the payload is equal to "ON" or "OFF"
                if (payload.equals(String(LIGHT_ON))) {
                        if (m_light_state != true) {
                                m_light_state = true;
                                setLightState();
                                publishLightState();
                        }
                } else if (payload.equals(String(LIGHT_OFF))) {
                        if (m_light_state != false) {
                                m_light_state = false;
                                setLightState();
                                publishLightState();
                        }
                }
        }
}

bool reconnect() {  //MQTT reconnection procedure
        Serial.print("INFO: Attempting MQTT connection...");
        // Attempt to connect
        if (MQTTclient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
                Serial.println("INFO: connected");
                // Once connected, publish an announcement...
                publishLightState();
                // ... and resubscribe
                MQTTclient.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
                return true;
        } else {
                Serial.print("ERROR: failed, rc=");
                Serial.println(MQTTclient.state());
                return false;
        }
}
