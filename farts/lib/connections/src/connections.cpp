#include "connections.h"

void beginProgram(){
	Serial.begin(115200);
	delay(500);
    Serial.println("----- Begin Program");
}

void wifiConnect(const char* ssid, const char* password){
	
	Serial.println("----- Wifi");
	WiFi.mode(WIFI_STA);
	//Serial.println("  Mode Set");

	//WiFi.begin(ssid, password);
	//Serial.print("Connecting to SSID ");
	//Serial.print(ssid);
}

void wifiStableConnect(const char* ssid, const char* password){
	
	int retries = 0;
	wifiConnect(ssid, password);
	
	while(( WiFi.status() != WL_CONNECTED) && retries <= 5 )  { 
		Serial.print(".");
		retries++;
		delay(2000);
	}
	Serial.println("");
	if ( retries >= 5 ){
		Serial.println("no WiFi. Rebooting...");
		/*wifiRestart(ssid, password); */
		wifiDisconnect();
		ESP.restart();
	}
	Serial.print("Connected to network with IP address ");
	Serial.println(WiFi.localIP());
}

void wifiDisconnect(){
	Serial.println("Disconnecting Wifi...");
	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
	//delay(500);
}

void wifiRestart(const char* ssid, const char* password){
	Serial.println("Restarting Wifi...");
	wifiDisconnect();
	wifiConnect(ssid, password);
}

bool brokerConnect(PubSubClient& client, const char* mqtt_server, const char* mqtt_client){

	int retries = 0;

	Serial.println("----- MQTT");
  client.setServer(mqtt_server, 1883);

  Serial.print("Trying to connect to MQTT Broker ");
  Serial.print(mqtt_server);
  while (!client.connected()) {

    while ( !client.connect(mqtt_client)) {
		Serial.print(".");
		delay(500);
		retries++;
		
		if (retries >= 5){
			Serial.println("");
			Serial.println("Couldn't connect");
			return false;
			}
	}
	
  }
  
      Serial.println("");
      Serial.println("Connected");
	  return true;
}

void mqtt_publish( PubSubClient& client, const char* topic, const char* msg){
	Serial.print("publishing: ");
	Serial.print(topic);
	Serial.print(": ");
	Serial.println(msg);
  
	client.publish(topic, msg);
	delay(200);
  }