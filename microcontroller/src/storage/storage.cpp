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
	JsonDocument doc; // JSON object

    // Check if the WiFi credentials need to be updated
    doc["ssid"] = wiFiSSID;
    doc["password"] = wiFiPassword;

    // Add the API key to the JSON object
    doc["apiKey"] = apiKey;

    // Add the visibility settings to the JSON object
    doc["currentPrice"] = currentPriceVisible;
    doc["priceChange"] = priceChangeVisible;
    doc["marketCap"] = marketCapVisible;
    doc["dailyHighLow"] = dailyHighLowVisible;
    doc["yearHighLow"] = yearHighLowVisible;
    doc["openPrice"] = openPriceVisible;
    doc["volume"] = volumeVisible;

    // Add the various settings to the JSON object
    doc["formatType"] = formatType == FORMAT_US ? "US" : "EU";
    doc["matrixIntensity"] = matrixIntensity;
    doc["scrollSpeed"] = scrollSpeed;

    // Write data to EEPROM
    EepromStream eepromStream(0, 256);
	if (!serializeJson(doc, eepromStream)) {
        return false; // Error while writing on EEPROM
    }

    // Commit changes
	if (!EEPROM.commit()) {
        return false; // Error while committing changes
    }

    // Mark as saved
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

    // Load the API key
	if (!doc["apiKey"].isNull())
        stringCopy(apiKey, doc["apiKey"], sizeof(apiKey));
    
    // Load the WiFi credentials
    if (!doc["ssid"].isNull())
        stringCopy(wiFiSSID, doc["ssid"], sizeof(wiFiSSID));
    if (!doc["password"].isNull())
        stringCopy(wiFiPassword, doc["password"], sizeof(wiFiPassword));
    
    // Load the visibility settings
    if (!doc["currentPrice"].isNull())
        currentPriceVisible = doc["currentPrice"].as<bool>();
    if (!doc["priceChange"].isNull())
        priceChangeVisible = doc["priceChange"].as<bool>();
    if (!doc["marketCap"].isNull())
        marketCapVisible = doc["marketCap"].as<bool>();
    if (!doc["dailyHighLow"].isNull())
        dailyHighLowVisible = doc["dailyHighLow"].as<bool>();
    if (!doc["yearHighLow"].isNull())
        yearHighLowVisible = doc["yearHighLow"].as<bool>();
    if (!doc["openPrice"].isNull())
        openPriceVisible = doc["openPrice"].as<bool>();
    if (!doc["volume"].isNull())
        volumeVisible = doc["volume"].as<bool>();

    // Load the various settings
    if (!doc["formatType"].isNull())
        formatType = doc["formatType"].as<const char*>() == "US" ? FORMAT_US : FORMAT_EU;
    if (!doc["matrixIntensity"].isNull())
        matrixIntensity = doc["matrixIntensity"].as<uint8_t>();
    if (!doc["scrollSpeed"].isNull())
        scrollSpeed = doc["scrollSpeed"].as<uint8_t>();
    
	Serial.println("EEPROM data loaded successfully");
}

// Setup EEPROM
void setupEEPROM() {
	EEPROM.begin(256); // Start the EEPROM
    loadSettingFromEEPROM(); // Load the settings from the EEPROM
}