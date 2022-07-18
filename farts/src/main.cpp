#include <Arduino.h>
#include <connections.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include "time.h"

#define SEALEVELPRESSURE_HPA (1025)

unsigned long delayTime;

//Display Setting
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE); //Display

//Sensor Settings
Adafruit_BME280 bme;  //Sensor
float temp;
float humi;

//Wifi Settings
const char* ssid = "Bashers Kingdom";
const char* password = "scheisse23";

//Time Settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200;
const int   daylightOffset_sec = 3600;

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

void setup() {

  //Start
  Serial.begin(115200);
  Serial.println("BME280 Test");

  //Setup Display
  u8g2.begin();
  bool status;
  status = bme.begin(0x76);
  if(!status) {
    Serial.println("Couldnt find Sensor");
    while(1);
  }
  Serial.println("-- Default Test --");
  delayTime = 1000;
  Serial.println();

  //setup Wifi
  /*Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  } */
}

  


void loop() { 

  // get SensorValues
  char temp_str[16];
  char humi_str[16];

  sprintf(temp_str, "Temp: %.2f", bme.readTemperature());
  sprintf(humi_str, "Humi: %.2f", bme.readHumidity());

  //Print on Display
  u8g2.firstPage();
  do {

    u8g2.setFont(u8g2_font_fur11_tf);


    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.drawStr(0,18,temp_str);
    u8g2.drawStr(0,40,humi_str);

  } while ( u8g2.nextPage() );

  //wait for next Values
  delay(delayTime);
}