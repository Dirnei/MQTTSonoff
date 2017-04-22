#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* WiFi Settings */
const char* ssid     = "YOURWIFINAME";
const char* password = "YOURPASSWORD";

/* MQTT Settings */
const char* mqttTopic1 = "/room1/main/light1";       // topic 1 --> relay 1   
const char* mqttTopic2 = "/room1/main/light2";       // topic 2 --> relay 2
const char* mqttTopic3 = "/room1/main/light3";       // topic 3 --> relay 3
const char* mqttTopic4 = "/room1/main/light4";       // topic 4 --> relay 4

const char* mqttPubPrefix =   "/status";             // MQTT topic for publishing relay state
#define ENABLE_FEEDBACK                              // enables the mqtt status feedback after changing relay state

IPAddress broker(192,168,0,106);                      // Address of the MQTT broker
#define CLIENT_ID "client-1c6adc"                    // Client ID to send to the broker

// uncomment USE_MQTT_AUTH if you want to connect anonym
#define USE_MQTT_AUTH
#define MQTT_USER "mqttuser"
#define MQTT_PASSWORD "***********"

/* Sonoff Device */
#define SONOFF_1CH
//#define SONOFF_2CH    //currently not supported
//#define SONOFF_4CH
//#define SONOFF_TOUCH  //currently not supported

#ifdef SONOFF_1CH
  const int relayPins[] = {12};
  int relayStates[] = {0};
  const int statusLedPin= 13;  
#endif

#ifdef SONOFF_4CH
  const int relayPins[] = {12,5,4,15};
  int relayStates[] = {0,0,0,0};
  const int statusLedPin = 13;
#endif


WiFiClient wificlient;
PubSubClient client(wificlient);
volatile int relayState = 1;



void setup() 
{
  /* Set up LED and Relay. LED is active-low */
  pinMode(statusLedPin, OUTPUT);

#ifdef SONOFF_1CH
  pinMode(relayPin, OUTPUT);
#endif

#ifdef SONOFF_4CH
  pinMode(relayPins[0], OUTPUT);
  pinMode(relayPins[1], OUTPUT);
  pinMode(relayPins[2], OUTPUT);
  pinMode(relayPins[3], OUTPUT);
#endif

  setRelay(0);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  reconnectWifi();
  
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /* Prepare MQTT client */
  client.setServer(broker, 1883);
  client.setCallback(mqttMessage);


  pinMode(0,INPUT);
  pinMode(9,INPUT);
  pinMode(10,INPUT);
  pinMode(14,INPUT);
  
  attachInterrupt(0, buttonInterrupt1, RISING);
  attachInterrupt(9, buttonInterrupt2, RISING);
  attachInterrupt(10, buttonInterrupt3, RISING);
  attachInterrupt(14, buttonInterrupt4, RISING);
}

void buttonInterrupt1()
{
  setRelay(0, -1, (char*)mqttTopic1);
}


void buttonInterrupt2()
{
  setRelay(1, -1, (char*)mqttTopic2);
}

void buttonInterrupt3()
{
  setRelay(2, -1, (char*)mqttTopic3);
}

void buttonInterrupt4()
{
  setRelay(3, -1, (char*)mqttTopic4);
}

void loop() 
{
  if (WiFi.status() != WL_CONNECTED)
  {
    reconnectWifi();
  }
  else if (!client.connected())
  {
    reconnectMQTT();
  }
  else  
  {
    client.loop();
  }
}

void reconnectWifi()
{
  int counter = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    // blinking LED while connecting...
    if(counter++ % 2 == 0)
    {
      digitalWrite(statusLedPin, HIGH);
    }
    else
    {
      digitalWrite(statusLedPin,LOW);
    }
      
    // also putting a . on the serial for debugging
    Serial.print(".");

    if(counter > 30)
    { 
      // to many connecting attempts.. try to restart
      Serial.println("");
      Serial.println("Restarting...");
      ESP.restart();
    }
    
    delay(500);
  }
}

void reconnectMQTT() 
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting to connect to MQTT broker...");
    // Attempt to connect
#ifdef USE_MQTT_AUTH
    if (client.connect(CLIENT_ID, MQTT_USER, MQTT_PASSWORD))
#else
    if(cleint.connect(CLIENT_ID))
#endif
    {
      Serial.println("connected to MQTT broker");
      client.subscribe(mqttTopic1);
      client.subscribe(mqttTopic2);
      client.subscribe(mqttTopic3);
      client.subscribe(mqttTopic4);
    } 
    else 
    {
      Serial.print(" Reconnect failed. State=");
      Serial.println(client.state());
      Serial.println("Retry in 3 seconds...");
      // Wait 5 seconds before retrying
      delay(3000);
    }
  }
}

void mqttMessage(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  
  if (!strcmp(topic, mqttTopic1)) 
  {
    setRelay(0, getDesiredRelayState(payload, length), topic);
  }
  else if (!strcmp(topic, mqttTopic2))
  {
    setRelay(1, getDesiredRelayState(payload, length), topic);
  }
  else if (!strcmp(topic, mqttTopic3))
  {
    setRelay(2, getDesiredRelayState(payload, length), topic);
  }
  else if (!strcmp(topic, mqttTopic4))
  {
    setRelay(3, getDesiredRelayState(payload, length), topic);
  }
}

int getDesiredRelayState(byte* payload, unsigned int length)
{
    if (!strncasecmp_P((char *)payload, "OFF", length)) 
    {
      return 0;
    }
    else if (!strncasecmp_P((char *)payload, "ON", length)) 
    {
      return 1;
    }
    else if (!strncasecmp_P((char *)payload, "TOGGLE", length))
    {
      return -1;
    }
}

void setRelay(int state)
{
  #ifdef SONOFF_1CH
  setRelay(relayPin, ledPin, state);
  #endif
}

void setRelay(int pinIndex, int state, char* topic)
{

#ifdef SONOFF_1CH
    pinIndex = 0;
#endif

    int pin = relayPins[pinIndex];
    
    if (state == -1)
    {
      state = !relayStates[pinIndex];
    }
    
    digitalWrite(pin, state);
    relayStates[pinIndex] = state;
    
#ifdef SONOFF_1CH
    digitalWrite(statusLedPin, !state);
#endif

#ifdef ENABLE_FEEDBACK

    char mqttPubTopic[128];
    sprintf(mqttPubTopic, "%s%s", topic, mqttPubPrefix);
    
    if(state == 1)
    {
      Serial.println("ON");
    }
    else
    { 
      Serial.println("OFF");
    }

#endif
}




