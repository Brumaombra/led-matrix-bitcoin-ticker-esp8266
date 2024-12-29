#include "storage/storage.h"
#include "api/api.h"
#include "wifi/wifi.h"
#include "matrix/matrix.h"
#include "server/server.h"

// Setup
void setup() {
	Serial.begin(9600); // Start serial
	setupLittleFS(); // Setup LittleFS
	setupEEPROM(); // Setup EEPROM
	setupServer(); // Setup server
	manageWiFiConnection(); // Manage WiFi connection
	setupWebClient(); // Setup web client
	setupLedMatrix(); // Setup LED matrix
}

// Loop
void loop() {
	manageWiFiConnection(); // Manage WiFi connection
	manageLedMatrix(); // Manage the LED matrix
}