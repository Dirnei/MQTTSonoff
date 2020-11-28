#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <stdint.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

class Channel
{
  public:
    Channel(){};
    Channel(uint8_t index, uint8_t pin, uint8 buttonPin);

    void setRelay(int8_t state);
    void buttonPress(void);
    uint8_t getButtonPin();
    bool getState();
    bool hasStateChanged();
    bool sateRequested();
    void subscribe(const char *baseTopic);
    bool handlePayload(char *topic, JsonDocument &doc);
    void loop();

  protected:
    uint8_t _index;
    uint8_t _pin;
    uint8_t _buttonPin;
    bool _state;
    bool _stateChanged;
    bool _stateRequested;
    bool _needsReset;
    uint _resetAt;
};

extern PubSubClient _client;

#endif