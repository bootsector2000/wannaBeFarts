#include "Client.h"
#include <PubSubClient.h>
#include <WiFi.h>

void beginProgram();
void wifiStableConnect (const char*, const char* );
void wifiConnect(const char*, const char*);
void wifiDisconnect();
void wifiRestart(const char*, const char*);
bool brokerConnect(PubSubClient&, const char*, const char*);

void mqtt_publish(PubSubClient&, const char*, const char*);