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
   Version 1.2 - Alex Muthmann - https://dev-eth0.de

   DESCRIPTION
   Temperature and Pressure Sensor using Bosch's BMP280. It uses the Adafruit Library and provides both Temperature and current Pressure in seperate readings.

   NOTE
   If you want to run your Arduino with batteries, you should disable the brownout using an ISP:
   bin\avrdude.exe -Cetc\avrdude.conf -v -patmega328p -cstk500v1 -PCOM9 -b19200 -U lfuse:w:0xff:m -U hfuse:w:0xda:m -U efuse:w:0xff:m
*/


//////////////////////////////////////////////////////
///////////////// NODE CONFIGURATION /////////////////
//////////////////////////////////////////////////////
#define MY_DEBUG // Enable debug prints to serial monitor


//#define BME280; // Enable if you are using a BME280 instead of BMP280

// disable this if you don't want to measure the voltage
#define BATTERY_SENSE_PIN A0  // select the input pin for the battery sense point
#define SEND_VOLTAGE; // Enable if you want to send the voltage explicit
#define VMIN 1.9  // Battery Level 0%
#define VMAX 3.3  // Batter Level 100%
#define VBAT_PER_BITS 0.003363075  // Volts per bit on the A0 pin. For 470k and 1M Ohm @ 3.3V

// Enable and select radio type attached
#define MY_RADIO_NRF24
#define MY_RF24_CE_PIN 9
#define MY_RF24_CS_PIN 10

// If debugging is enabled, ignore issues with a missing gateway
#ifdef MY_DEBUG
  #define MY_TRANSPORT_WAIT_READY_MS 1
#endif

#include <math.h>
#include <SPI.h>
#include <MySensors.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#ifdef BME280 
  #include <Adafruit_BME280.h>
  Adafruit_BME280 sensor;
  uint8_t addr = 0x76;
#else 
  #include <Adafruit_BMP280.h>
  Adafruit_BMP280 sensor;
  uint8_t addr = 0x76;
#endif

// Sleep time between reads (in milliseconds)
#ifdef MY_DEBUG
  const unsigned long SLEEP_TIME = 5000; // debug? update every 5 seconds
#else
  const unsigned long SLEEP_TIME = 300000; // once every 5 minutes
#endif


float lastPressure = -1;
float lastTemp = -1;
float lastVolt = -1;
float lastHumidity = -1;
bool initialSetup = true;
// IDs used
#define CHILD_ID_TEMP V_TEMP
#define CHILD_ID_BARO V_PRESSURE
#define CHILD_ID_BAT V_VOLTAGE
#define CHILD_ID_HUM V_HUM

MyMessage tempMsg(CHILD_ID_TEMP, V_TEMP);
MyMessage pressureMsg(CHILD_ID_BARO, V_PRESSURE);
MyMessage pressureMsgPrefix(CHILD_ID_BARO, V_UNIT_PREFIX);
#ifdef BATTERY_SENSE_PIN
#ifdef SEND_VOLTAGE
MyMessage voltMsg(CHILD_ID_BAT, V_VOLTAGE);
#endif
#endif

#ifdef BME280
  MyMessage humidityMsg(CHILD_ID_HUM, V_HUM);
#endif



void setup()
{
  Serial.begin(57600);
  Serial.println("setup");
  if (!sensor.begin(addr))
  {
    Serial.println("Could not find a valid Sensor, check wiring!");
    while (1) {}
  }
  analogReference(INTERNAL);
}

void presentation()  {
  Serial.println("presentation");

  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Pressure Sensor", "1.3");

  // Register sensors to gw
  present(CHILD_ID_TEMP, S_TEMP);
  present(CHILD_ID_BARO, S_BARO);
  #ifdef BATTERY_SENSE_PIN
    present(CHILD_ID_BAT, S_MULTIMETER);
  #endif
  #ifdef BME280
    present(CHILD_ID_HUM, S_HUM);
  #endif
}

void loop()
{
  Serial.println("loop");
  // Read pressure as mBar and temperature
  float pressure = sensor.readPressure()/100;
  float temperature = sensor.readTemperature();
  #ifdef BME280
    float humidity = sensor.readHumidity();
  #endif
  
  #ifdef BATTERY_SENSE_PIN
    // Read voltage
    int sensorValue = analogRead(BATTERY_SENSE_PIN);    // Battery monitoring reading
    float volt  = sensorValue * VBAT_PER_BITS;
    int batteryPcnt = sensorValue / 10;
  #endif
  
  // Debug Output
  #ifdef MY_DEBUG
    Serial.print("Temperature = "); Serial.print(temperature); Serial.println(" *C");
    Serial.print("Pressure = "); Serial.print(pressure); Serial.println(" hPa");
    #ifdef BME280
      Serial.print("Humidity = "); Serial.print(humidity); Serial.println(" %");
    #endif
    #ifdef BATTERY_SENSE_PIN
      Serial.print("A0 analogRead = "); Serial.println(sensorValue);
      Serial.print("Battery Voltage= "); Serial.print(volt); Serial.println(" V");
      Serial.print("Battery percent= "); Serial.print(batteryPcnt); Serial.println(" %");
    #endif
  #endif

  // Send values to gateway
  if(!isnan(temperature)){
    send(tempMsg.set(temperature, 1));
    initialSetup = false;
  }
  if(!isnan(pressure)){
    send(pressureMsgPrefix.set("hPa"));  // Set fixed unit
    send(pressureMsg.set(pressure, 2));
    initialSetup = false;
  }
  #ifdef BME280
    if(!isnan(humidity)){
      send(humidityMsg.set(humidity, 0));
      initialSetup = false;
    }
  #endif
  #ifdef BATTERY_SENSE_PIN
    #ifdef SEND_VOLTAGE
    send(voltMsg.set(volt, 2));
    #endif
    sendBatteryLevel(batteryPcnt);
  #endif
  
  // only sleep, if initial values were send
  if(!initialSetup){
    sleep(SLEEP_TIME);
  }
}


