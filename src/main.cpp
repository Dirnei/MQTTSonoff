#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Channel.h>

/* MQTT Settings */
char _basetopic[] = "Your/base/topic";
char _mqttBroker[] = "192.168.0.251";

#define WIFI_SSID "Your-SSID"
#define WIFI_PASS "YourSecurePassword"

#define CLIENT_ID "client-0000" // Client ID to send to the broker

// uncomment USE_MQTT_AUTH if you want to connect anonym
//#define USE_MQTT_AUTH

#define MQTT_USER "useranme"
#define MQTT_PASSWORD "password"

/* Sonoff Device */
//#define SONOFF_1CH
//#define SONOFF_2CH    //currently not supported
#define SONOFF_4CH
//#define SONOFF_TOUCH  //currently not supported

#ifdef SONOFF_1CH
#define CHANNEL_COUNT 1
Channel _channels[] = {Channel(0, 12, 0)};
#endif
// 0,9,10,14
#ifdef SONOFF_4CH
#define CHANNEL_COUNT 4
Channel _channels[] = {Channel(0, 12, 0), Channel(1, 5, 9), Channel(2, 4, 10), Channel(3, 15, 14)};
#endif

const int statusLedPin = 13;
WiFiClient wificlient;
PubSubClient _client(wificlient);

std::vector<std::string> _topics;

void onMqttMessage(char *topic, uint8_t *payload, unsigned int length)
{
    Serial.printf(" >> %s\n", topic);
    StaticJsonDocument<128> doc;

    DeserializationError result = deserializeJson(doc, payload);
    if (result)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(result.c_str());
        return;
    }

    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        if (_channels[i].handlePayload(topic + strlen(_basetopic), doc))
        {
            Serial.println(" [HANDELED]");
            return;
        }
    }

    Serial.println("----------------------------------------------------------------------------------\n");
}

void setup()
{
    Serial.begin(115200);

    /* Set up LED and Relay. LED is active-low */
    pinMode(statusLedPin, OUTPUT);
    digitalWrite(statusLedPin, HIGH);

    WiFi.mode(WIFI_STA);
    _client.setServer(_mqttBroker, 1883);
    _client.setCallback(onMqttMessage);
}

uint _nextOutput = 0;

void reconnectWifi()
{
    int counter = 0;
    WiFi.disconnect();
    delay(200);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        // blinking LED while connecting...
        digitalWrite(statusLedPin, counter++ % 2 == 0);

        // also putting a . on the serial for debugging
        Serial.print(".");

        if (counter > 30)
        {
            // to many connecting attempts.. try to restart
            Serial.println("=======================");
            Serial.println("      Restarting");
            Serial.println("=======================");

            ESP.restart();
        }

        delay(500);
    }

    digitalWrite(statusLedPin, LOW);
}

void publishState()
{
    StaticJsonDocument<128> doc;
    doc["state"] = "online";

    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        char channel[10];
        snprintf(channel, 16, "channel_%d", i);
        doc[channel] = _channels[i].getState();
    }

    char buf[128];
    serializeJson(doc, buf);
    _client.publish(_basetopic, buf);
    Serial.print(" [All Channels] > Publishing channel state\n");
}

void publishChannelState(int index)
{
    StaticJsonDocument<32> doc;
    doc["state"] = _channels[index].getState();

    char buf[32];
    serializeJson(doc, buf);
    char topic[64];
    snprintf(topic, 64, "%s/channel_%d", _basetopic, index);

    Serial.printf(" [Channel %d] > Publishing channel state\n", index);
    _client.publish(topic, buf);
}

void reconnectMQTT()
{
    // Loop until we're reconnected
    while (!_client.connected())
    {
        Serial.print("Attempting to connect to MQTT broker...");

        // Attempt to connect
#ifdef USE_MQTT_AUTH
        if (_client.connect(CLIENT_ID, MQTT_USER, MQTT_PASSWORD))
#else
        if (_client.connect(CLIENT_ID))
#endif
        {
            Serial.println("connected to MQTT broker");

            for (int i = 0; i < CHANNEL_COUNT; i++)
            {
                _channels[i].subscribe(_basetopic);
                publishChannelState(i);
            }

            publishState();
        }
        else
        {
            Serial.print(" Reconnect failed. State=");
            Serial.println(_client.state());
            Serial.println("Retry in 3 seconds...");
            // Wait 5 seconds before retrying
            delay(3000);
        }
    }
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        reconnectWifi();
    }
    else if (!_client.connected())
    {
        reconnectMQTT();
    }
    else
    {
        static bool stateChanged = false;
        for (int i = 0; i < CHANNEL_COUNT; i++)
        {
            if (_channels[i].hasStateChanged())
            {
                stateChanged = true;
                publishChannelState(i);
            }
            else if(_channels[i].sateRequested())
            {
                publishChannelState(i);
            }
        }

        if (stateChanged)
        {
            stateChanged = false;
            publishState();
        }

        _client.loop();
    }
}