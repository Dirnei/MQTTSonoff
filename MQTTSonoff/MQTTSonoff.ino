#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* WiFi Settings */
const char* ssid     = "ENTER SSID";
const char* password = "AND PASSWORD";

/* Sonoff Outputs */
const int relayPin = 12;  // Active high
const int ledPin   = 13;  // Active low

/* MQTT Settings */
const char* mqttTopic = "/room1/main/light";          // MQTT topic
const char* mqttPub =   "/room1/main/light/status";   // MQTT topic for publishing relay state
IPAddress broker(192,168,0,10);                       // Address of the MQTT broker
#define CLIENT_ID "client-1c6adc"                     // Client ID to send to the broker

#define USE_MQTT_AUTH
#define MQTT_USER "admin"
#define MQTT_PASSWORD "********"

WiFiClient wificlient;
PubSubClient client(wificlient);
volatile int relayState = 1;

void setup() 
{
  /* Set up LED and Relay. LED is active-low */
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

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
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      digitalWrite(ledPin,LOW);
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
      client.subscribe(mqttTopic);
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

  
  if (!strcmp(topic, mqttTopic)) 
  {
    if (!strncasecmp_P((char *)payload, "OFF", length)) 
    {
      setRelay(0);
    }
    else if (!strncasecmp_P((char *)payload, "ON", length)) 
    {
      setRelay(1);
    }
    else if ( ! strncasecmp_P((char *)payload, "TOGGLE", length))
    {
      setRelay(!relayState);
    }
  }
}

void setRelay(int state)
{
  if(relayState != state)
  {
    relayState = state;
    digitalWrite(relayPin, relayState);
    digitalWrite(ledPin, !relayState);

    if(relayState == 1)
    {
      client.publish(mqttPub, "ON"); 
    }
    else
    {
      client.publish(mqttPub, "OFF"); 
    }
  }
}




