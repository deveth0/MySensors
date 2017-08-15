/**
 * This Sketch is based on the MySensors Pressure Sensor Sketch which uses an older version of the Sensor (BMP085).
 * https://github.com/mysensors/MySensorsArduinoExamples/tree/master/examples/PressureSensor
 *
 * Original Headers:
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 * Version 1.1 - Alex Muthmann - https://dev-eth0.de
 *
 * DESCRIPTION
 * Temperature and Pressure Sensor using Bosch's BMP280. It uses the Adafruit Library and provides both Temperature and current Pressure in seperate readings.
 */


// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
#define MY_RF24_CE_PIN 9
#define MY_RF24_CS_PIN 10

#define VBAT_PER_BITS 0.003363075  // Calculated volts per bit from the used battery montoring voltage divider.   Internal_ref=1.1V, res=10bit=2^10-1=1023, Eg for 3V (2AA): Vin/Vb=R1/(R1+R2)=470e3/(1e6+470e3),  Vlim=Vb/Vin*1.1=3.44V, Volts per bit = Vlim/1023= 0.003363075
#define VMIN 1.9  // Battery monitor lower level. Vmin_radio=1.9V
#define VMAX 3.3  //  " " " high level. Vmin<Vmax<=3.44

#include <SPI.h>
#include <MySensors.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Sleep time between reads (in milliseconds)
const unsigned long SLEEP_TIME = 60000;


Adafruit_BMP280 bmp;
float lastPressure = -1;
float lastTemp = -1;

// IDs used
#define CHILD_ID 40

MyMessage tempMsg(CHILD_ID, V_TEMP);
MyMessage pressureMsg(CHILD_ID, V_PRESSURE);


int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;


void setup()
{
	if (!bmp.begin(0x76))
	{
		Serial.println("Could not find a valid BMP280 sensor, check wiring!");
		while (1) {}
	}
  analogReference(INTERNAL);
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Pressure Sensor", "1.1");

  // Register sensors to gw
  present(CHILD_ID, S_BARO);
  present(CHILD_ID, S_TEMP);
}

void loop()
{
	float pressure = bmp.readPressure();
	float temperature = bmp.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" hPa");
    
	if (temperature != lastTemp)
	{
		send(tempMsg.set(temperature, V_TEMP));
		lastTemp = temperature;
	}

	if (pressure != lastPressure)
	{
		send(pressureMsg.set(pressure, V_PRESSURE));
		lastPressure = pressure;
	}

  int sensorValue = analogRead(BATTERY_SENSE_PIN);    // Battery monitoring reading
  Serial.println(sensorValue);
  float Vbat  = sensorValue * VBAT_PER_BITS;
  Serial.print("Battery Voltage: "); Serial.print(Vbat); Serial.println(" V");
  //int batteryPcnt = sensorValue / 10;
  int batteryPcnt = static_cast<int>(((Vbat-VMIN)/(VMAX-VMIN))*100.);   
  Serial.print("Battery percent: "); Serial.print(batteryPcnt); Serial.println(" %");

	sleep(SLEEP_TIME);
}


