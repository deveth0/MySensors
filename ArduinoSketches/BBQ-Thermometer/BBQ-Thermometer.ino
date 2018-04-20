// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
#define MY_RF24_CE_PIN 9
#define MY_RF24_CS_PIN 10

// Sleep time between reads (in milliseconds)
unsigned long SLEEP_TIME = 3000;

#include <MySensors.h>

// Thermistor
#define THERMISTORPIN A0         
// the value of 'R2' resistor
#define SERIESRESISTOR 10000    

// Steinhart Coefficients
float a = 0.2476197866e-03, b = 2.943226015e-04, c = -2.129189836e-07;

// ID used
#define CHILD_ID 23
MyMessage tempMsg(CHILD_ID,V_TEMP);



void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("BBQ Sensor", "1.1");

  // Register all sensors to gw
  present(CHILD_ID, S_TEMP);
}
 
void loop(void) {
  analogRead(THERMISTORPIN);
  
  float analog = analogRead(THERMISTORPIN);
  // Calculate the actual resistance
  float R1 = SERIESRESISTOR * (1023.0 / analog - 1.0);
  // Calculate the temperature
  float steinhart;
  steinhart = a;                                            // A
  steinhart += b * log(R1);                             // + B * ln(R)
  steinhart += c * log(R1) * log(R1)* log(R1);  // + C * (ln(R))^3
  steinhart = 1.0 / steinhart;                              // 1/(...)

  float temperature = steinhart - 273.15;                   // convert to °C

  // Debug Output
  #ifdef MY_DEBUG
    Serial.print("Temperature = "); Serial.print(temperature); Serial.println(" *C"); 
  #endif
  send(tempMsg.set(temperature, 1));
  sleep(SLEEP_TIME);
}
