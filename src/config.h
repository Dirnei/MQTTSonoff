
#ifndef _CONFIG_H_
#define _CONFIG_H_

/* MQTT Settings */
char _basetopic[] = "Your/base/topic";
char _mqttBroker[] = "192.168.0.251";
bool _retainStates = true;

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

#endif