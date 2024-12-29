#include "server.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "../wifi/wifi.h"
#include "../config/config.h"
#include "../utils/utils.h"

// Setup delle route
void setupRoutes() {
	server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html"); // Serve web page
	server.onNotFound([](AsyncWebServerRequest *request) { // Page not found
		request->send(404);
	});

	// Connect to WiFi
	server.on("/connect", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (!request->hasParam("ssid") || !request->hasParam("password")) { // Check required fields
			request->send(400, "application/json", "{\"status\":\"KO\"}"); // Response
			return;
		}
		stringCopy(wiFiSSID, request->getParam("ssid")->value().c_str(), 35); // Save the SSID
		stringCopy(wiFiPassword, request->getParam("password")->value().c_str(), 70); // Save the password
		Serial.print("SSID: ");
		Serial.println(wiFiSSID);
		Serial.print("Password: ");
		Serial.println(wiFiPassword);
		wiFiConnectionStatus = WIFI_TRY; // Trying to connect
		char jsonResponse[20]; // JSON response
		snprintf(jsonResponse, sizeof(jsonResponse), "{\"status\":\"%d\"}", wiFiConnectionStatus); // Create response
		request->send(200, "application/json", jsonResponse); // Response
		newWiFiCredentials = true; // New credentials
	});

	// Check the WiFi connection status
	server.on("/checkConnection", HTTP_GET, [](AsyncWebServerRequest *request) {
		char jsonResponse[20]; // JSON response
		snprintf(jsonResponse, sizeof(jsonResponse), "{\"status\":\"%d\"}", wiFiConnectionStatus);
		switch(wiFiConnectionStatus) {
			case WIFI_TRY:
				request->send(200, "application/json", jsonResponse); // Response
				break;
			case WIFI_OK:
				disableAccessPoint = true; // I need to disable the access point
				request->send(200, "application/json", jsonResponse); // Response
				break;
			case WIFI_KO:
				request->send(400, "application/json", jsonResponse); // Response
				break;
		}
	});

	// Send the list of networks
	server.on("/networks", HTTP_GET, [](AsyncWebServerRequest *request) {
		WiFi.scanNetworksAsync([request](int numNetworks) {
			JsonDocument doc; // JSON object
			JsonArray networks = doc["networks"].to<JsonArray>();
			for (int i = 0; i < numNetworks; i++) {
				JsonObject network = networks.add<JsonObject>();
				network["ssid"] = WiFi.SSID(i);
				network["signal"] = WiFi.RSSI(i);
			}
			size_t jsonLength = measureJson(doc) + 1; // Get the size of the JSON object
			char json[jsonLength];
			serializeJson(doc, json, jsonLength);
			request->send(200, "application/json", json); // Send the JSON object
		});
    });

	// Save the API key
	server.on("/apiKey", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (!request->hasParam("apiKey")) { // Check if the API key is missing
        	request->send(400, "application/json", "{\"status\":\"KO\"}"); // Send the JSON object
        	return;
    	}
		stringCopy(apiKey, request->getParam("apiKey")->value().c_str(), 35); // Save the API key
		request->send(200, "application/json", "{\"status\":\"OK\"}"); // Send the JSON object
		Serial.println("API key changed");
		newApiKey = true; // New API key
	});

	// Get the values visibility settings
	server.on("/valuesSettings", HTTP_GET, [](AsyncWebServerRequest *request) {
		JsonDocument doc; // JSON object
		doc["currentPrice"] = currentPriceVisible ? "Y" : "N";
		doc["priceChange"] = priceChangeVisible ? "Y" : "N";
		doc["marketCap"] = marketCapVisible ? "Y" : "N";
		doc["dailyHighLow"] = dailyHighLowVisible ? "Y" : "N";
		doc["yearHighLow"] = yearHighLowVisible ? "Y" : "N";
		doc["openPrice"] = openPriceVisible ? "Y" : "N";
		doc["volume"] = volumeVisible ? "Y" : "N";
		doc["formatType"] = formatType == FORMAT_US ? "US" : "EU";
		size_t jsonLength = measureJson(doc) + 1; // Get the size of the JSON object
		char json[jsonLength];
		serializeJson(doc, json, jsonLength);
		request->send(200, "application/json", json); // Send the JSON object
	});

	// Save the values visibility settings
	server.on("/saveValuesSettings", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (request->hasParam("currentPrice"))
			currentPriceVisible = request->getParam("currentPrice")->value() == "Y";
		if (request->hasParam("priceChange"))
			priceChangeVisible = request->getParam("priceChange")->value() == "Y";
		if (request->hasParam("marketCap"))
			marketCapVisible = request->getParam("marketCap")->value() == "Y";
		if (request->hasParam("dailyHighLow"))
			dailyHighLowVisible = request->getParam("dailyHighLow")->value() == "Y";
		if (request->hasParam("yearHighLow"))
			yearHighLowVisible = request->getParam("yearHighLow")->value() == "Y";
		if (request->hasParam("openPrice"))
			openPriceVisible = request->getParam("openPrice")->value() == "Y";
		if (request->hasParam("volume"))
			volumeVisible = request->getParam("volume")->value() == "Y";
		if (request->hasParam("formatType"))
			formatType = request->getParam("formatType")->value() == "US" ? FORMAT_US : FORMAT_EU;
		request->send(200, "application/json", "{\"status\":\"OK\"}");
		Serial.println("Values settings changed");
	});
}

// Setup server
void setupServer() {
	setupRoutes(); // Setup routes
	server.begin(); // Start the server
}