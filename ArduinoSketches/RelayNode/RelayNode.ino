/**
   
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

 *******************************

   REVISION HISTORY
   Version 1.0 - Alex Muthmann - https://dev-eth0.de

   DESCRIPTION
   Relay Node

*/


//////////////////////////////////////////////////////
///////////////// NODE CONFIGURATION /////////////////
//////////////////////////////////////////////////////
#define MY_DEBUG // Enable debug prints to serial monitor
#define MY_NODE_ID 113

// Enable and select radio type attached
#define MY_RADIO_NRF24
#define MY_RF24_CE_PIN 10
#define MY_RF24_CS_PIN 11

// If debugging is enabled, ignore issues with a missing gateway
#ifdef MY_DEBUG
  #define MY_TRANSPORT_WAIT_READY_MS 1
#endif

#include <MySensors.h>

// Relay configuration
#define RELAY_PIN 5  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define NUMBER_OF_RELAYS 2 // Total number of attached relays
#define RELAY_ON LOW  // GPIO value to write to turn on attached relay
#define RELAY_OFF HIGH // GPIO value to write to turn off attached relay

// Sleep time between reads (in milliseconds)
#ifdef MY_DEBUG
  const unsigned long SLEEP_TIME = 1000; // debug? update every half second
#else
  const unsigned long SLEEP_TIME = 1000; // once every half second
#endif


bool initialValueSent = false;

// IDs used
#define CHILD_ID_RELAIS_1 2
#define CHILD_ID_RELAIS_2 3

MyMessage relay1Msg(CHILD_ID_RELAIS_1, V_STATUS);
MyMessage relay2Msg(CHILD_ID_RELAIS_2, V_STATUS);

void setup()
{
  Serial.begin(115200);
  Serial.println("setup");
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN + 1, OUTPUT);
}

void presentation()  {
  Serial.println("presentation");
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Relay Node", "1.0");
  // Register relay to gw
  present(CHILD_ID_RELAIS_1, S_BINARY);
  present(CHILD_ID_RELAIS_2, S_BINARY);
  
  // Disable both relays
  digitalWrite(RELAY_PIN, RELAY_OFF);
  digitalWrite(RELAY_PIN + 1 , RELAY_OFF);

}

void loop()
{  
  // send initial value for relay
  if (!initialValueSent) {
    // Set state to RELAY_OFF
    send(relay1Msg.set(RELAY_OFF));    
    Serial.println("Requesting initial value from controller");
    request(CHILD_ID_RELAIS_1, V_STATUS);
    wait(2000, C_SET, V_STATUS);
    send(relay2Msg.set(RELAY_OFF));
    request(CHILD_ID_RELAIS_2, V_STATUS);
    wait(2000, C_SET, V_STATUS);
  }
  
  delay(SLEEP_TIME);
}

void receive(const MyMessage &message)
{
    Serial.println("Receive Message");
    // We only expect one type of message from controller. But we better check anyway.
    if (message.type==V_STATUS) {
      if (!initialValueSent) {
        Serial.println("Receiving initial value from controller");
        initialValueSent = true;
      }
        // Change relay state
        Serial.println("Received message for "+ message.sensor);
        if(message.sensor ==  CHILD_ID_RELAIS_1){
          digitalWrite(RELAY_PIN, message.getBool() ? RELAY_ON:RELAY_OFF);
          send(relay1Msg.set(message.getBool() ? 1 : 0)); 
        }
        else if(message.sensor == CHILD_ID_RELAIS_2){
          digitalWrite(RELAY_PIN + 1, message.getBool() ? RELAY_ON:RELAY_OFF);
          send(relay2Msg.set(message.getBool() ? 1 : 0)); 
        }
    }
}
