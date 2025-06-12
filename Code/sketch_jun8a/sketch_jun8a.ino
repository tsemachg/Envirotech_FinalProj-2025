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
#include <Wire.h>
#include <SPI.h>
//#include <SD.h>
#include <Arduino.h>                           // required before wiring_private.h
#include <Notecard.h>                          // Include the Arduino library for the Notecard
#include "Adafruit_PM25AQI.h"
#include <Seeed_HM330X.h>
#include <RTClib.h> 


//notehub define
#define productUID "il.ac.bgu.post.bonehh:final_project_envirotech"            //the unique Product Identifier for your device - set at notehub
#define myProductID productUID
Notecard notecard;

//Battery voltage (Feather M0)
//#define VBATPIN A7            //Bat pin feather
float Feather_V = 0;             //float variable for adalogger voltage
float NoteCard_V = 0;       //float variable for Notecarrier voltage
float Notecard_temp = 0;

//RTC_DS3231 rtc;                           //RTC define
char buffer [24];                           //char var used to hold time and date
uint8_t secq, minq, hourq, dayq, monthq;    //uint8_t var used to hold the sec, min, hour, day and month
uint16_t yearq;

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
  notecard.setDebugOutputStream(Serial);      // enables to watch JSON objects being transferred to and from the Notecard for each request on the serial terminal window
  notecard.begin();                           //start notecard with I2C protocol 
  
  Serial.begin(115200);

  //Note card setup
  // Configure Notecard to synchronize with Notehub periodically, as well as adjust the frequency based on the battery level
  {
    J * req = notecard.newRequest("hub.set");
    JAddStringToObject(req, "product", productUID);                                     //Assign a Notehub ProductUID to the Notecard
    JAddStringToObject(req, "mode", "periodic");                                        //The Notecard will periodically power-on the modem and connect to Notehub whenever it needs to synchronize with the service, and will then power-off the modem when not in use
    JAddStringToObject(req, "vinbound", "usb:2;high:60;normal:120;low:240;dead:0");     //The max wait time (in minutes) to sync inbound data from Notehub - with a voltage-variable value 
    JAddStringToObject(req, "voutbound", "usb:2;high:30;normal:60;low:120;dead:0");     //The max wait time (minutes) to sync outbound data (such as new Notes added by the host application) from the Notecard - with a voltage-variable value
    //JAddStringToObject(req, "card.motion.mode", "stop");  
    //JAddBoolToObject(req, "sync", true);                                                //If you want to initiate a sync immediately after adding a Note, set the sync field to true.                                           
    notecard.sendRequestWithRetry(req, 5);                                               // Issue the request
  }
  //Optimize voltage variable behaviors for LiPo battery
  {
    J * req = notecard.newRequest("card.voltage");
    JAddStringToObject(req, "mode", "lipo");                                  // "lipo" for LiPo batteries. Equivalent to "usb:4.6; high:4.0; normal:3.5; low:3.2; dead:0"             
    notecard.sendRequest(req);
  }


  //delay(3000); // wait for sensor to boot up
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
      //while (1);
  }
  Serial.println("Grove sensor found!");
  
  DateTime b(nc_time());
  DateTime bfixed = b + TimeSpan(0, 2, 0, 0);
  secq = bfixed.second();
  minq = bfixed.minute();
  hourq = bfixed.hour();
  dayq = bfixed.day();
  monthq = bfixed.month(); 
  yearq = bfixed.year(); 
  sprintf (buffer, "%02u:%02u:%02u %02u/%02u/%04u", hourq, minq, secq, dayq, monthq, yearq);
  Serial.print(buffer);

  //Establish a template to optimize queue size and data usage
  {
  J * req = notecard.newRequest("note.template");
  JAddStringToObject(req, "file", "readings.qo");
  J * body = JAddObjectToObject(req, "body");
  JAddStringToObject(body, "Time", "15:12:24 17/08/2023");
  JAddNumberToObject(body, "Notecard_temp", 12.1);
  JAddNumberToObject(body, "Feather_V", 12.1);
  JAddNumberToObject(body, "NoteCard_V", 12.1);
  JAddNumberToObject(body, "Adafruit1", 12.1);
  JAddNumberToObject(body, "Adafruit2.5", 12.1);
  JAddNumberToObject(body, "Adafruit10", 12.1);
  JAddNumberToObject(body, "Grove1", 12.1);
  JAddNumberToObject(body, "Grove2.5", 12.1);
  JAddNumberToObject(body, "Grove10", 12.1);

  notecard.sendRequest(req);
  Serial.print("built template!,   ");
  }


  // ------------------------------ "loop" inside setup ---------------------------------
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
    //return;
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
  
  J *rsp = notecard.requestAndResponse(notecard.newRequest("card.voltage"));                     //NoteCard voltage measurement 
  if (rsp != NULL) {
      NoteCard_V = JGetNumber(rsp, "value");
      notecard.deleteResponse(rsp);
  }
  Notecard_temp = Get_NoteCard_Temp();


  const char* time = buffer;
  J *req = notecard.newRequest("note.add");
  if (req != NULL)  {
    JAddStringToObject(req, "file", "readings.qo");
    //JAddBoolToObject(req, "sync", true);                  //when uncomment will sync to notehub every reading
    J *body = JAddObjectToObject(req, "body");
    if (body)  {
      JAddStringToObject(body, "Time", time);
      JAddNumberToObject(body, "Notecard_temp", Notecard_temp);
      JAddNumberToObject(body, "Feather_V", Feather_V);
      JAddNumberToObject(body, "NoteCard_V", NoteCard_V);
      JAddNumberToObject(body, "Adafruit1", data.pm10_env);
      JAddNumberToObject(body, "Adafruit2.5", data.pm25_env);
      JAddNumberToObject(body, "Adafruit10", data.pm100_env);
      JAddNumberToObject(body, "Grove1", pm10);
      JAddNumberToObject(body, "Grove2.5", pm25);
      JAddNumberToObject(body, "Grove10", pm100);
    }
    notecard.sendRequest(req);
    
  }
  


  

}

void loop()  {
  int IntervalSeconds = getInterval();
  int TimeOfSensorsWarmingUp = 15;       //              //[sec] this means it will wake up for ~20 sec every cycle and then wait another ~10 seconds until meeting the interval condition (i.e., letting about 10 seconds for the CO2 and O2(only from dfrobot, the figaro is most likely not affected by warming time) warming up time ); needs to be optimized for the time of the acual readigs (i.e., how much time the adalogger spend in the setup loop)
  int IntervalSecondsCorrected = IntervalSeconds-TimeOfSensorsWarmingUp;
  Serial.print("entering sleep mode for "); Serial.print(IntervalSecondsCorrected); Serial.println(" seconds");
  //delay(100);
  J * req = NoteNewCommand("card.attn");
  JAddStringToObject(req, "mode", "sleep");
  JAddNumberToObject(req, "seconds", IntervalSecondsCorrected);
  notecard.sendRequest(req);
  // Wait 1s before retrying
  //Watchdog.disable(); // end the 25 seconds timer
  ::delay(1000);
}


unsigned long nc_time() {
	//unsigned long linux_epoch;
  uint32_t t;
	J *req, *rsp;
	if ((req = notecard.newRequest("card.time")) && (rsp = notecard.requestAndResponse(req))){
		t = JGetNumber(rsp, "time");
		notecard.deleteResponse(rsp);
		return t;
	}
	return 0;
} 

float Get_NoteCard_Temp() {
    float result = 0.0f;
    // Query Notecard for temperature
    J * req = notecard.newRequest("card.temp");
    J * rsp = notecard.requestAndResponse(req);

    // Parse response
    if (JHasObjectItem(rsp,"value")) {
        J * temp_obj = JGetObjectItem(rsp,"value");
        if (JIsNumber(temp_obj)) {
            result = static_cast<float>(JNumberValue(temp_obj));
        }
    }
    notecard.deleteResponse(rsp);
    return result;
}

// This function returns the reading_interval from the environment variables in noteHub
int getInterval() {
  int IntervalSeconds = 600;                                    //a default value -(in seconds)- If the variable is not set, set to 0, or set to an invalid type 
  J *req = notecard.newRequest("env.get");
  if (req != NULL) {
      JAddStringToObject(req, "name", "reading_interval");
      J* rsp = notecard.requestAndResponse(req);
      int readingIntervalEnvVar = atoi(JGetString(rsp, "text"));
      if (readingIntervalEnvVar > 0) {
        IntervalSeconds = readingIntervalEnvVar;
      }
      notecard.deleteResponse(rsp);
  }
  return IntervalSeconds;
}