# MQTTSonoff
A small and simple firmware for the Sonoff Devices to add MQTT functionallity to it.

## Motivation

I started this project because the other solutions i found were to big for that what i wan't to use them. This should contain only the essentials for what these devices are for.

### Supported devices

- Sonoff CH1
- Sonoff CH4

## Platformio

I did a complete rewrite of the code after a switch to PlatformIO. I kept the original device support, but made it easier to add new ones.

## How to use

If you wan't to use this firmware you have to clone this repository and change some variables in the main.cpp

```cpp

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

```

## MQTT Topics

### Status Topic

> Your/base/topic

It will be published after it is connected and on every change of the relays. Payload is as follows:

```json
{
  "state": "online",
  "ip": "192.168.0.125",
  "channel_0": false,
  "channel_1": false,
  "channel_2": false,
  "channel_3": false
}
```

### Channel Topic

> Your/base/topic/channel_**:index**

It will be published after it is connected and on every change of the relay. Payload is as follows:

```json
{
  "state": "on"
}
```

### Refresh/Request channel state

> Your/base/topic/channel_**:index**/get

If you want to get the current state published again you can send an empty object to this topic.

```json
{}
```

### Set channel

> Your/base/topic/channel_**:index**/set

It will be published after it is connected and on every change of the relay. Payload is as follows:

```json
{
  "state": "toggle",
  "resetIn": 10000
}
```

#### Sate

Valid values are `on`, `off`, `toggle`.

#### ResetIn _(optional)_

If it is set, the relay state will be turned off after the given timeout in milliseconds.