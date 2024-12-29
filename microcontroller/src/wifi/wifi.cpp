#include "wifi.h"
#include <Arduino.h>
#include "../config/config.h"
#include "../storage/storage.h"
#include "../matrix/matrix.h"

// Connecting to WiFi
bool connectToWiFi() {
	WiFi.begin(wiFiSSID, wiFiPassword); // Connecting to the WiFi
	byte maxTry = 50; // Maximum number of attempts to connect to WiFi
	byte count = 0; // Counter
	Serial.print("Connecting to WiFi");
	while (WiFi.status() != WL_CONNECTED) {
		if (count >= maxTry) {
			wiFiSSID[0] = '\0'; // Reset network SSID
			wiFiPassword[0] = '\0'; // Reset network password
			return false; // Connection failed
		}
		count++;
		Serial.print(".");
		delay(250);
	}
	Serial.println(" connected!");
	writeEEPROM(); // Save the network credentials
	return true; // Connection success
}

// Setting up the access point
bool setupAccessPoint() {
	if (accessPointEnabled) // Check if already enabled
		return true; // If enabled exit the function
	accessPointEnabled = WiFi.softAP(accessPointSSID); // Start the access point
	if (!accessPointEnabled) // Check if enabled
		return false; // If not enabled exit the function
	return true; // Access point enabled
}

// Manage WiFi connection
bool manageWiFiConnection() {
	if (wiFiConnectionStatus == WIFI_TRY) { // Check if already trying to connect
		if (connectToWiFi()) { // Connecting to WiFi
			wiFiConnectionStatus = WIFI_OK; // Update connection status
			return true; // Connection success
		} else {
			wiFiConnectionStatus = WIFI_KO; // Update connection status
			return false; // Connection failed
		}
	}

	// Every 2 seconds
	if (millis() - timestampWiFiConnection > 2000) {
		timestampWiFiConnection = millis(); // Save timestamp
		if (disableAccessPoint && accessPointEnabled) { // Check if I need to disable the access point
			accessPointEnabled = !WiFi.softAPdisconnect(); // Disable access point
			if (!accessPointEnabled) // Check if disabled
				disableAccessPoint = false; // Mark as disabled
		}

		if (WiFi.status() == WL_CONNECTED) // Check if connected to WiFi
			return true; // If connected exit the function
			
		if (wiFiSSID[0] != '\0' && wiFiPassword[0] != '\0') { // Check if credentials are already present
			if (connectToWiFi()) // Connecting to WiFi
				return true; // Connection success
			else
				setupAccessPoint(); // Setup access point
		} else {
			setupAccessPoint(); // Setup access point
		}
		return false; // Connection failed
	}
	return true; // Connection success
}

// Check if connected to WiFi
bool checkWifiConnection() {
	if (WiFi.status() != WL_CONNECTED) {
		const char errorMessage[] = "Not connected to Wi-Fi. Use the 'Bitcoin-Ticker' access point to enter the Wi-Fi credentials.";
		Serial.println(errorMessage);
		printOnLedMatrix(errorMessage, sizeof(errorMessage)); // Print the message on the matrix
		return false; // Not connected
	}
	return true; // Connected
}