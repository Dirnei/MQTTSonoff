#include <Channel.h>
#include <functional>

Channel::Channel(uint8_t index, uint8_t pin, uint8_t buttonPin)
{
    _index = index;
    _pin = pin;
    _buttonPin = buttonPin;

    pinMode(_pin, OUTPUT);
    pinMode(_buttonPin, INPUT);
}

void Channel::setRelay(int8_t state)
{
    if (state == -1)
    {
        state = !_state;
    }

    digitalWrite(_pin, state);
    _state = state;
    _stateChanged = true;
}

void Channel::buttonPress()
{
    setRelay(-1);
}

uint8_t Channel::getButtonPin()
{
    return _buttonPin;
}

bool Channel::getState()
{
    return _state;
}

bool Channel::hasStateChanged()
{
    bool changed = _stateChanged;
    _stateChanged = false;
    return changed;
}

bool Channel::sateRequested()
{
    bool requested = _stateRequested;
    _stateRequested = false;
    return requested;
}

void Channel::subscribe(const char *baseTopic)
{
    char topic[128];
    snprintf(topic, 128, "%s/channel_%d/+", baseTopic, _index);
    Serial.printf(" Subscribe to %s\n", topic);
    _client.subscribe(topic);
}

bool Channel::handlePayload(char *topic, JsonDocument &doc)
{
    char target[15];
    snprintf(target, 15, "/channel_%d/get", _index);
    if (strcmp(target, topic) == 0)
    {
        _stateRequested = true;
        return true;
    }

    snprintf(target, 15, "/channel_%d/set", _index);
    if (strcmp(target, topic) == 0)
    {
        if (!doc.containsKey("state"))
        {
            return true;
        }

        const char *state = doc["state"];
        if (strcasecmp(state, "on") == 0)
        {
            setRelay(1);
        }
        else if (strcasecmp(state, "off") == 0)
        {
            setRelay(0);
        }
        else if (strcasecmp(state, "toggle") == 0)
        {
            setRelay(-1);
        }

        return true;
    }

    return false;
}