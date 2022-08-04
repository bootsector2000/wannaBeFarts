#include <Arduino.h>
#include <connections.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include "time.h"
#include <stdbool.h>
#include <PubSubClient.h>

#define SEALEVELPRESSURE_HPA 1025

#define OFF -1
#define OK 0
#define CONNECTING 2
#define ERROR 1
//#define SENSORFAIL 1

//Display Setting
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE); //Display

//Sensor Settings
Adafruit_BME280 bme;  //Sensor

//Wifi Settings
const char* ssid = "Bashers Kingdom";
const char* password = "scheisse23";

//MQTT Settings
const char* mqtt_server="192.168.178.25";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char mqtt_msg[50];
int value = 0;

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
  int mqtt = OFF;   //0-OK ; 1-Connecting ; 2-Failure
};
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

  Serial.print("mqttStatus :");
  Serial.println(s.mqtt);

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
    case CONNECTING:
      strcat(wifi_str, "Connecting...") ; 
      displayWifi = true; 
      break;
    
    case ERROR:
      strcat(wifi_str, "ERROR") ; 
      displayWifi = true; 
      break;

    case OK:
      strcat(wifi_str, "OK");
      displayWifi = false;
      break;

    case OFF:
      strcat(wifi_str, "OFF"); 
      displayWifi = true;
      break;

    default:
      strcat(wifi_str, "UNKNOW"); 
      displayWifi = true;
  }
  /*Serial.print("Wifi_str: ");
  Serial.println(wifi_str);*/

  switch(s.sensor){    // Sensorstatus anzeigen oder nicht
      case OFF:
        strcat(sensor_str, "OFF"); 
        displaySensor = true;
        break;
      case OK:
        strcat(sensor_str, "OK");
        displaySensor = false;
        break;
      case ERROR:
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
  char warnings[100] = "\0";

  int u = 0;
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

}

void displayString(const char* string){
  u8g2.firstPage(); //Display leeren
  do {
      u8g2.setFont(u8g2_font_fur11_tf);
      u8g2.drawStr(0, 16 ,string);
      
  } while ( u8g2.nextPage() );
}

// -------------------------------------------------------------------------------------------
// ------------ Run the Code -----------------------------------------------------------------
// -------------------------------------------------------------------------------------------
void setup() { 

  //Start
  Serial.println("Begin Program...");

  Serial.println("Setup...");
  Serial.begin(115200);
  delay(200);
  
  //Setup Display
  Serial.println("-Display...");
  u8g2.begin();

  //Setup Sensor
  Serial.println("-Sensor");
  bool status;
  status = bme.begin(0x76);
  if(!status) {
    Serial.println("--Couldnt find Sensor");
    s.sensor = ERROR;
  } else {
    Serial.println("--Sensor gefunden");
    s.sensor = OK;
  }

  //Setup WiFi
  Serial.println("-Wifi");
  
  Serial.print("--Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);
  s.wifi = CONNECTING;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    displayString("Wifi Connecting...");
  }
  s.wifi = OK;
  Serial.println();

  //Setup MQTT
  Serial.println("-MQTT");
  client.setServer(mqtt_server, 1883);
  Serial.print("--Connect MQTT");
  while (!client.connected()) {
    s.mqtt = CONNECTING;
    client.connect("TestConnection");
    displayString("MQTT Connect...");
    delay(500);
  }
  s.mqtt = OK;
  Serial.println();
  Serial.println("--MQTT Broker connected.");

Serial.println (" +++ Setup Complete +++"); 
}

void loop() { 

  int delayTime = 5000;

  //get SensorValues
  s.temp = bme.readTemperature();
  s.humi = bme.readHumidity();

  printStats(s);
  displayStats(s);

  if (client.connected()){

    //strcpy(mqtt_msg, "TestString");
    sprintf(mqtt_msg, "%.2f", s.temp);
    Serial.print("Publishing "); Serial.println(mqtt_msg);
    client.publish("/esp32/temp", mqtt_msg);

    sprintf(mqtt_msg, "%.2f", s.humi);
    Serial.print("Publishing "); Serial.println(mqtt_msg);
    client.publish("/esp32/humi", mqtt_msg);
    
  } else{
    s.mqtt = ERROR;
    Serial.println("Client not connected");
  }
  delay(delayTime);
}