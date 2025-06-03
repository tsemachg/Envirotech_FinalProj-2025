//  +---------------------------------------------------+
//  |                                                   |
//  |  2025-05-28                                       |
//  |                                                   |
//  |  Envirotech, BGU 2025 - Final Project             |
//  |  PM Sensor                                        |
//  |  Preliminary sketch                               |
//  |  Hagar Boneh, Stav Gil-Bar and Gill Tsemach       |
//  |                                                   |
//  |  Code is based on:                                |
//  |  * Adafruit's PM25 AQI Sensor library             |
//  |    PM25_test.ino                                  |
//  |                                                   |
//  |  * Example for Seeed PM2.5 Sensor(HM300)          |
//  |    basic_demo.ino                                 |
//  |    Copyright (c) 2018 Seeed Technology Co., Ltd.  |
//  |    Website    : www.seeed.cc                      |
//  |    Author     : downey                            |
//  |    Create Time: August 2018                       |
//  |                                                   |
//  +---------------------------------------------------+


// Loading libraries
#include "Adafruit_PM25AQI.h"
#include <Seeed_HM330X.h>

// // Grove sensor stuff 
// #ifdef  ARDUINO_SAMD_VARIANT_COMPLIANCE
//     #define SERIAL_OUTPUT Serial
// #else
//     #define SERIAL_OUTPUT Serial
// #endif

// Setting variables
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

HM330X sensor; 
uint8_t data2[30]; // Grove one makes strange measurement output length 30 
                   // and uses 7 from it..



void setup() {
  Serial.begin(115200);

  delay(3000); // wait for sensor to boot up
  Serial.println("----------Initilizing----------");

  Serial.println("Cheking components:");
  
  // Initialize Adafruit PMSA003I AQ sensor
  if (! aqi.begin_I2C()) {   // Try and connect by I2C
    Serial.println("Could not find Adafruit PMSA003I...");
    // Do something like abort?
  }
  Serial.println("Adafruit Sensor found!");

  // Initialize Grove - seedstudio HM3301 PM sensor
  if (sensor.init()) {
      Serial.println("HM330X init failed!!");
      while (1);
  }
  Serial.println("Grove sensor found!");

}

void loop() {
  PM25_AQI_Data data;  // adafruit
  uint16_t pm10 = 0;  // Grove
  uint16_t pm25 = 0;  // Grove
  uint16_t pm100 = 0;  // Grove

  // Read data
  //// Adafruit sensor
  if (! aqi.read(&data)) { 
    Serial.println("Could not read from AQI");
    
    delay(500);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");

  //// Grove sensor
  if (sensor.read_sensor_value(data2, 29)) {
    Serial.println("HM330X read result failed!!");
    
    delay(500);  // try again in a bit!
    return;
  } 
  Serial.println("HM330X reading success");

  // Serial.Print -> later save to SD / transmit blues.io
  //// Adafruit PMSA003I
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Adafruit -- Concentration Units (environmental)"));
  Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
  Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
  Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
  Serial.println("");

  //// Grove HM3301
  // parse_result_value(data2); not necessary for now

  // not exactly sure why, but this is how their code works...
  /*
  1 -> sensor number (?) "sensor num:"
  2,3,4 -> standard PM mass concentrations, industrial 
  5,6,7 -> concentration of PM in atmospheric environment
  So we use 5,6,7 of data2
  */
  pm10 = (uint16_t) data2[5 * 2] << 8 | data2[5 * 2 + 1];  // 5
  pm25 = (uint16_t) data2[6 * 2] << 8 | data2[6 * 2 + 1];  // 6
  pm100 = (uint16_t) data2[7 * 2] << 8 | data2[7 * 2 + 1]; // 7
  

  Serial.println(F("Grove -- Units (environmental) ug/m3"));
  Serial.print(F("PM 1.0: ")); Serial.print(pm10);
  Serial.print(F("\t\tPM 2.5: ")); Serial.print(pm25);
  Serial.print(F("\t\tPM 10: ")); Serial.println(pm100);
  
  Serial.println(F("---------------------------------------"));

  // ### this part created while experimenting
  /*
  uint16_t value = 0;
  for (int i = 5; i < 8; i++) {
      value = (uint16_t) data2[i * 2] << 8 | data2[i * 2 + 1];
      Serial.println("-------------------------------------------------------");
      Serial.print("value:         "); Serial.println(value);
      Serial.print("i:             "); Serial.println(i);
      Serial.print("i*2:           "); Serial.println(i*2);
      Serial.print("i*2+1:         "); Serial.println(i*2 + 1);
      Serial.print("buf[i*2]:      "); Serial.println(buf[i*2]);
      Serial.print("buf[i*2+1]:    "); Serial.println(buf[i*2 + 1]);
      Serial.print("buf[i*2] << 8: "); Serial.println(buf[i*2] << 8);
  }
  */


  delay(10000); // measure each 10 seconds

}
