#include <Arduino.h>
#include <connections.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include "time.h"
#include <stdbool.h>

#define SEALEVELPRESSURE_HPA 1025
#define OFF -1
#define OK 0
#define WIFICONNECTING 1
#define WIFIFAIL 2
#define SENSORFAIL 1

//Display Setting
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE); //Display

//Sensor Settings
Adafruit_BME280 bme;  //Sensor
/*float temp;
float humi;*/

//Wifi Settings
const char* ssid = "Bashers Kingdom";
const char* password = "scheisse23";

//Time Settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200;
const int   daylightOffset_sec = 3600;


struct stats
{
  float temp; //Temperatur in °C
  float humi; //Luftfeuchte in %
  int sensor = OFF; //0-OK ; 1-Failure
  int wifi = OFF;   //0-OK ; 1-Connecting ; 2-Failure
} ;
stats s;

void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void printStats(stats s) {

  Serial.print("Sensorstatus : ");
  Serial.println(s.sensor);

  Serial.print("WiFistatus : ");
  Serial.println(s.wifi);

  Serial.print("temp : ");
  Serial.println(s.temp);

  Serial.print("Humi : ");
  Serial.println(s.humi);
}

void displayStats(stats s){

  int offsetcnt = 1;        //wie oft soll Offset verwendet werden
  const int offset = 16;    //Fontgröße
  bool displayWifi, displaySensor;

  char wifi_str[20] = "Wifi: ";
  char sensor_str[20] = "Sensor: ";

  switch(s.wifi){ //Wifi anzeigen oder nicht
    case WIFICONNECTING:
      strcat(wifi_str, "Connecting...") ; 
      displayWifi = true; 
      break;
    
    case WIFIFAIL:
      strcat(wifi_str, "ERROR") ; 
      displayWifi = true; 
      break;

    case OK:
      strcat(wifi_str, "OK"); break;
      displayWifi = false;

    case OFF:
      strcat(wifi_str, "OFF"); 
      displayWifi = true;
      break;

    default:
      strcat(wifi_str, "UNKNOW"); 
      displayWifi = true;
  }
  Serial.print("Wifi_str: ");
  Serial.println(wifi_str);

  switch(s.sensor){    // Sensorstatus anzeigen oder nicht
      case OFF:
        strcat(sensor_str, "OFF"); 
        displaySensor = true;
        break;
      case OK:
        strcat(sensor_str, "OK");
        displaySensor = false;
        break;
      case SENSORFAIL:
        strcat(sensor_str, "ERROR");
        displaySensor = true;
        break;
      default:
      strcat(sensor_str, "UNKNOW"); 
      displaySensor = true;
  }

  //--- Ausgabestrings 
  char temp_str[16];
  char humi_str[16];
  sprintf(temp_str, "Temp: %.2f", s.temp);
  sprintf(humi_str, "Humi: %.2f", s.humi);

  //Anzeigen
  char warnings[100] = "";

  int u;
  u8g2.firstPage(); //Display leeren
  do {
      u8g2.setFont(u8g2_font_fur11_tf);

    if (displayWifi){
      strcat(warnings, wifi_str);

    }
    if (displaySensor){
      if (strlen(warnings) > 0){
        strcat(warnings, " -- ");
      }
      strcat(warnings, sensor_str);
    }

    if (strlen(warnings) > 0 ) {
      u8g2.drawStr(127-u, offsetcnt*offset,warnings); offsetcnt++;  
      u+=5;
    }

    u8g2.drawStr(0, offsetcnt*offset ,temp_str); offsetcnt++;
    u8g2.drawStr(0,offsetcnt *offset ,humi_str); offsetcnt++;

    if (u > ( 127 + strlen(warnings)*7) ) {
      u = 0;
    }

  } while ( u8g2.nextPage() );

  //Debugging
  Serial.println(" ------ Debugging DisplayStats ---------");
  Serial.print("-- Sensor_str: "); Serial.println(sensor_str);
  Serial.print("-- warnungen: "); Serial.print(warnings); Serial.print (" --- Größe: "); Serial.println( strlen(warnings));
  Serial.print("-- OffsetCnt :"); Serial.println(offsetcnt);
  Serial.print("-- u: "); Serial.println(u);
}

// -------------------------------------------------------------------------------------------
// ------------ Run the Code -----------------------------------------------------------------
// -------------------------------------------------------------------------------------------
void setup() { 

  //Start
  Serial.begin(115200);
  delay(200);
  Serial.println("Begin Program...");

  //Setup Display
  u8g2.begin();

  //Setup Sensor
  bool status;
  status = bme.begin(0x76);
  if(!status) {
    Serial.println("Couldnt find Sensor");
    s.sensor = SENSORFAIL;
  } else {
    Serial.println("Sensor gefunden");
    s.sensor = OK;
  }
  Serial.println("bla");
  Serial.println ("-- Setup Complete--");

  delay(1000);  

  /*setup Wifi
  /Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  } */
}

void loop() { 

  int delayTime = 200;

  //get SensorValues
  s.temp = bme.readTemperature();
  s.humi = bme.readHumidity();

  printStats(s);
  displayStats(s);

  delay(delayTime);
}