/**
   This Sketch is based on the MySensors Pressure Sensor Sketch which uses an older version of the Sensor (BMP085).
   https://github.com/mysensors/MySensorsArduinoExamples/tree/master/examples/PressureSensor

   Original Headers:
   Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
   Copyright (C) 2013-2015 Sensnology AB
   Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors

   Documentation: http://www.mysensors.org
   Support Forum: http://forum.mysensors.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.

 *******************************

   REVISION HISTORY
   Version 1.0 - Henrik Ekblad
   Version 1.1 - Alex Muthmann - https://dev-eth0.de

   DESCRIPTION
   Moisture Sensor using a i2c Sensor using the I2CSensor_std Library: https://ex-store.de/Kapazitiver-Feuchtigkeits-und-Temperatursensor-mit-I2C-Interface-und-Schaltausgang

*/


//////////////////////////////////////////////////////
///////////////// NODE CONFIGURATION /////////////////
//////////////////////////////////////////////////////
#define MY_DEBUG // Enable debug prints to serial monitor
#define MY_NODE_ID 112

// Enable and select radio type attached
#define MY_RADIO_NRF24
#define MY_RF24_CE_PIN 9
#define MY_RF24_CS_PIN 10

// If debugging is enabled, ignore issues with a missing gateway
#ifdef MY_DEBUG
  //#define MY_TRANSPORT_WAIT_READY_MS 1
#endif

#include <MySensors.h>
#include <I2CSensor_std.h>
#include <Wire.h>

// I2C Moisture Sensor
I2CSensor sensor(0x71);

// Sleep time between reads (in milliseconds)
#ifdef MY_DEBUG
  const unsigned long SLEEP_TIME = 5000; // debug? update every 5 seconds
#else
  const unsigned long SLEEP_TIME = 5000; // once every 5 seconds
#endif


float lastTemp = -1;
float lastHumidity = -1;
bool initialSetup = true;

// IDs used
#define CHILD_ID_TEMP V_TEMP
#define CHILD_ID_HUM V_HUM

MyMessage tempMsg(CHILD_ID_TEMP, V_TEMP);
MyMessage humidityMsg(CHILD_ID_HUM, V_HUM);

void setup()
{
  Serial.begin(115200);
  Serial.println("setup");
  Wire.begin();
  sensor.begin();
  
}

void presentation()  {
  Serial.println("presentation");
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Moisture Sensor", "1.1");
  // Register sensors to gw
  present(CHILD_ID_TEMP, S_TEMP);
  present(CHILD_ID_HUM, S_HUM);
}

void loop()
{
  Serial.println("loop");
  // Read pressure as mBar and temperature
  float temperature = sensor.getTemperature() / (float) 10;
  float humidity = sensor.getCapacitanceProz();
  
  // Debug Output
  #ifdef MY_DEBUG
    Serial.print("Temperature = "); Serial.print(temperature); Serial.println(" *C");
    Serial.print("Humidity = "); Serial.print(humidity); Serial.println(" %");
  #endif

  // Send values to gateway
  if(!isnan(temperature)){
    Serial.println("Temperature changed");
    send(tempMsg.set(temperature, 1));
    initialSetup = false;
  }
  if(!isnan(humidity)){
    Serial.println("Humidity changed");
    send(humidityMsg.set(humidity, 0));
    initialSetup = false;
  }
  // only sleep, if initial values were send
  if(!initialSetup){
    sleep(SLEEP_TIME);
  }
}

