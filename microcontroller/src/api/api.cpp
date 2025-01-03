#include "api.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "../config/config.h"
#include "../utils/utils.h"
#include "../storage/storage.h"
#include "../matrix/matrix.h"
#include "../serial/serial.h"

// HTTPS client
WiFiClientSecure clientSecure;
HTTPClient http; // HTTP object
const char apiUrl[] = "https://financialmodelingprep.com/stable/quote?symbol=BTCUSD&apikey="; // API URL

// Setup the web client
void setupWebClient() {
    clientSecure.setInsecure(); // Disable SSL certificate verification
    http.setTimeout(5000); // Set timeout
}

// Create the scrolling message
void createStockDataMessage(JsonDocument doc) {
	printLogfln("Creating message...");
	const byte MAX_NUMBER_SIZE = 30; // Max length for the numbers
	char tempString[MAX_NUMBER_SIZE]; // Temporary string 1
	char tempString2[MAX_NUMBER_SIZE]; // Temporary string 2
	double tempVal; // Temporary variable 1
	double tempVal2; // Temporary variable 2

	// Price
	tempVal = doc[0]["price"].as<double>();
	tempVal2 = doc[0]["changePercentage"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	formatCurrency(tempVal2, tempString2, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessagePrice, BUF_SIZE, " btc $ %s  (%s%%)", tempString, tempString2);

	// Daily Change
	tempVal = doc[0]["change"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageDailyChange, BUF_SIZE, "Daily Change: $ %s", tempString);

	// Market cap
	tempVal = doc[0]["marketCap"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageMarketCap, BUF_SIZE, "Market Cap: $ %s", tempString);

	// Daily High/Low
	tempVal = doc[0]["dayHigh"].as<double>();
	tempVal2 = doc[0]["dayLow"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	formatCurrency(tempVal2, tempString2, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageDailyHighLow, BUF_SIZE, "Daily High: $ %s  -  Daily Low: $ %s", tempString, tempString2);

	// Year High/Low
	tempVal = doc[0]["yearHigh"].as<double>();
	tempVal2 = doc[0]["yearLow"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	formatCurrency(tempVal2, tempString2, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageYearHighLow, BUF_SIZE, "Year High: $ %s  -  Year Low: $ %s", tempString, tempString2);

	// Open
	tempVal = doc[0]["open"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageOpen, BUF_SIZE, "Open: $ %s", tempString);

	// Volume
	tempVal = doc[0]["volume"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageVolume, BUF_SIZE, "Volume: $ %s", tempString);
}

// Getting Bitcoin data
bool getStockDataAPI() {
	char url[150]; // Full URL
	sprintf(url, "%s%s", apiUrl, apiKey); // Add the API key to the URL
	printLogfln(url); // Print the URL
	http.begin(clientSecure, url); // Start the connection with HTTPS client
	if (http.GET() == HTTP_CODE_OK) {
		printLogfln("Response body: %s", http.getString().c_str());
		JsonDocument doc; // Create the JSON object
		DeserializationError error = deserializeJson(doc, http.getString()); // Deserialize the JSON object
		if (error) { // Error while parsing the JSON
			printLogfln("Error while parsing the JSON: %s\n", error.c_str());
			http.end();
			return false;
		}
		createStockDataMessage(doc); // Create the scrolling message
		http.end(); // Close connection
		return true;
	} else {
		printLogfln("HTTP call error: %d\n", http.GET());
		http.end();
		return false;
	}
}

// Call the API to get the data
bool callAPI() {
	currentMillis = millis();
	if (currentMillis - timestampStockData > 360000 || timestampStockData == 0) { // Call the API every 6 minutes (To limit usage)
		// Check if the API key is present
		if (apiKey[0] == '\0') {
			const char errorMessageApi[] = "API key is not present. Use the web page to insert the key and try again.";
			printLogfln(errorMessageApi);
			printOnLedMatrix(errorMessageApi, sizeof(errorMessageApi)); // Print the message on the matrix
			return false; // If error, return false
		}

		printLogfln("Calling the API");

		// Get the data
		if (!getStockDataAPI()) {
			const char errorMessageServer[] = "Error while calling the API. Retrying...";
			printLogfln(errorMessageServer);
			printOnLedMatrix(errorMessageServer, sizeof(errorMessageServer)); // Print the message on the matrix
			return false; // If error, return false
		}

		// Update the timestamp
		timestampStockData = currentMillis;
	}
	return true; // If no error, return true
}