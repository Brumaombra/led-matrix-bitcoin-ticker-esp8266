#include "storage.h"
#include <EEPROM.h>
#include <StreamUtils.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "../config/config.h"
#include "../utils/utils.h"

// Setup LittleFS
bool setupLittleFS() {
	if (!LittleFS.begin()) { // Check if LittleFS is mounted
		Serial.println("An Error has occurred while mounting LittleFS");
		return false;
	} else {
		return true;
	}
}

// Read data from the EEPROM
bool readEEPROM(JsonDocument& doc) {
    EepromStream eepromStream(0, 256);
	DeserializationError error = deserializeJson(doc, eepromStream);
    if (error) {
        Serial.print("Error while reading the EEPROM: ");
        Serial.println(error.c_str()); // Print the error message
        return false;
    }
    return true; // Read success
}

// Write data to the EEPROM
bool writeEEPROM() {
    // Check if the data needs to be saved
    if (!newApiKey && !newWiFiCredentials) {
        return true;
    }

	EepromStream eepromStream(0, 256);
	JsonDocument doc;

    // Read data from EEPROM
	if (!readEEPROM(doc)) {
        Serial.println("Overwriting the saved data...");
    }

    // Check if the API key needs to be updated
	if (newApiKey) {
        doc["apiKey"] = apiKey;
    }

    // Check if the WiFi credentials need to be updated
	if (newWiFiCredentials) {
		doc["ssid"] = wiFiSSID;
		doc["password"] = wiFiPassword;
	}

    // Write data to EEPROM
	if (!serializeJson(doc, eepromStream)) {
        return false; // Error while writing on EEPROM
    }

    // Commit changes
	if (!EEPROM.commit()) {
        return false; // Error while committing changes
    }

    // Mark as saved
	newApiKey = false;
	newWiFiCredentials = false;
	Serial.println("Data saved on EEPROM");
	return true; // Write success
}

// Load the settings from the EEPROM
void loadSettingFromEEPROM() {
    JsonDocument doc; // JSON object

    // Read the EEPROM
	if (!readEEPROM(doc)) { 
        return; // If error, exit the function
    }

    // Check if the API key is present
	if (!doc["apiKey"].isNull()) {
        stringCopy(apiKey, doc["apiKey"], sizeof(apiKey));
    }
    
    // Check if the WiFi SSID is present
    if (!doc["ssid"].isNull()) {
        stringCopy(wiFiSSID, doc["ssid"], sizeof(wiFiSSID));
    }
    
    // Check if the WiFi password is present
    if (!doc["password"].isNull()) {
        stringCopy(wiFiPassword, doc["password"], sizeof(wiFiPassword));
    }
    
	Serial.println("EEPROM data loaded");
}

// Setup EEPROM
void setupEEPROM() {
	EEPROM.begin(256); // Start the EEPROM
    loadSettingFromEEPROM(); // Load the settings from the EEPROM
}