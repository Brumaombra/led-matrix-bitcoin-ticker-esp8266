#include <MD_Parola.h>
#include <MD_MAX72xx.h>
// #include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
// #include <WiFiClientSecure.h>
// #include <WiFiClient.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <currency.h>

AsyncWebServer server(80); // Web server
// WiFiClientSecure client; // Client object
WiFiClient client; // Client object
HTTPClient http; // HTTP object
const char accessPointSSID[] = "Bitcoin-Ticker"; // Access point SSID
char wiFiSSID[35] = ""; // Network SSID
char wiFiPassword[70] = ""; // Network password
bool accessPointEnabled = false; // If access point enabled
bool disableAccessPoint = false; // If I need to disable the access point
enum connectionStatus { WIFI_TRY = 2, WIFI_OK = 1, WIFI_KO = 0 }; // Connection status
connectionStatus wiFiConnectionStatus = WIFI_KO; // Connection status
unsigned long currentMillis; // Current time
unsigned long timestampStockData = 0; // Timestamp stock data
unsigned long timestampWiFiConnection = 0; // Timestamp WiFi connection
enum formatNum { FORMAT_US = 1, FORMAT_EU = 2 }; // Numeric formatting type
formatNum formatType = FORMAT_US; // Current numeric formatting type
enum printType { PRINT_PRICE = 0, PRINT_CHANGE = 1, PRINT_HIGH_LOW = 2, PRINT_OPEN = 3 }; // Print type
printType switchText = PRINT_PRICE; // Variable for the switch
char apiKey[35] = "8b5e75db3ac93df1144f0743f2b5b786"; // Your API key for financialmodelingprep.com <- TODO - Change with your data

#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)

// Hardware configuration (Pinout)
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16 // Number of modules <- TODO - Change with your data
#define CLK_PIN 14 // SCK <- TODO - Change with your data (If needed)
#define DATA_PIN 13 // MOSI <- TODO - Change with your data (If needed)
#define CS_PIN 15 // SS <- TODO - Change with your data (If needed)

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
uint8_t scrollDelay = 30; // Matrix refresh delay
textEffect_t scrollEffect = PA_SCROLL_LEFT; // Scrolling effect
textPosition_t scrollAlign = PA_LEFT; // Scroll direction
uint16_t scrollPause = 0; // Pause at the end of scrolling

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE 250 // Buffer length
char curMessage[BUF_SIZE]; // Current message
char newMessage[BUF_SIZE]; // New message
char stripMessagePrice[BUF_SIZE]; // Price
char stripMessageDailyChange[BUF_SIZE]; // Change
char stripMessageHighLow[BUF_SIZE]; // Year High/Low
char stripMessageOpen[BUF_SIZE]; // Open
bool newMessageAvailable = true; // New available message

// Custom string copy function
void stringCopy(char* destination, const char* text, int length) {
    strncpy(destination, text, length - 1); // Copy the string
    destination[length - 1] = '\0'; // Add the terminating character
}

// Print the message on the matrix
void printOnLedMatrix(const char* message, const byte stringLength, uint16_t messageStill = scrollPause) {
	stringCopy(newMessage, message, stringLength); // Copy the message
	P.displayText(newMessage, scrollAlign, scrollDelay, messageStill, scrollEffect, scrollEffect); // Print the message on the matrix
	newMessageAvailable = true; // New message available
}

/* Replace dots and commas
String replaceDotsAndCommas(String input) {
  	for (unsigned int i = 0; i < input.length(); i++) {
		if (input.charAt(i) == '.') {
			input.setCharAt(i, ',');
		} else if (input.charAt(i) == ',') {
			input.setCharAt(i, '.');
		}
	}
  	return input;
}
*/

/*
void formatStringNumber2(char* numberToFormat) {
	long winds=17040;
	char buf[21];
	if (winds>=1000)
		snprintf(buf,sizeof(buf),"%ld,%03ld",winds/1000,winds%1000);
	else
		snprintf(buf,sizeof(buf),"%ld",winds);
	Serial.println(buf);
}

void *formatStringNumber3(unsigned long val, char *s) {
    char *p = s + 13;
    *p = '\0';
    do {
        if ((p - s) % 4 == 2)
            *--p = ',';
        *--p = '0' + val % 10;
        val /= 10;
    } while (val);
    return p;
}
*/

/* Formatting number
String formatStringNumber(String numberToFormat) {
    byte pointIndex = numberToFormat.indexOf("."); // Add or remove decimals if needed
    if (pointIndex < 0) { // No decimals => 2000
        numberToFormat += ".00";
    } else if (numberToFormat.length() - pointIndex > 3) { // Too many decimals => 2000.85648
        numberToFormat = numberToFormat.substring(0, numberToFormat.indexOf(".") + 3);
    } else if (numberToFormat.length() - pointIndex < 3) { // Too few decimals => 2000.5
        numberToFormat += "0";
    } else { // Unsupported format
        return numberToFormat;
    }

    // Add thousands separator
    byte currLength = 6;
    while (numberToFormat.length() > currLength) {
        numberToFormat = numberToFormat.substring(0, numberToFormat.length() - currLength) + "," + numberToFormat.substring(numberToFormat.length() - currLength);
        currLength += 4;
    }

	// If setted for EU, replace points and commas
	if (formatType == FORMAT_EU)
		numberToFormat = replaceDotsAndCommas(numberToFormat);
    return numberToFormat;
}
*/

/* Formatting number
void formatStringNumber(char* numberToFormat) {
	const int MAX_STRING_SIZE = 50;
    byte length = strlen(numberToFormat);
    char temp[MAX_STRING_SIZE];
    memset(temp, 0, MAX_STRING_SIZE);

    // Add or remove decimals as needed
    char* pointIndex = strchr(numberToFormat, '.'); // The position of "."
    if (!pointIndex) { // No decimals => 2000
        snprintf(temp, MAX_STRING_SIZE, "%s.00", numberToFormat); // Add decimals
    } else {
        byte decimalCount = length - (pointIndex - numberToFormat + 1); // Number of decimals
        if (decimalCount > 2) { // Too many decimals => 2000.85648
			// stringCopy(temp, numberToFormat, pointIndex - numberToFormat + 3);
            strncpy(temp, numberToFormat, pointIndex - numberToFormat + 3);
            temp[pointIndex - numberToFormat + 3] = '\0';
        } else if (decimalCount < 2) { // Too few decimals => 2000.5
            snprintf(temp, MAX_STRING_SIZE, "%s0", numberToFormat);
        } else { // Unsupported format
            // strncpy(temp, numberToFormat, MAX_STRING_SIZE);
			stringCopy(temp, numberToFormat, MAX_STRING_SIZE);
        }
    }

    // Add thousand separator
    int currLength = 6;
    int tempLength = strlen(temp);
    while (tempLength > currLength) {
        for (int i = tempLength; i >= tempLength - currLength; --i)
            temp[i + 1] = temp[i];
        temp[tempLength - currLength] = ',';
        currLength += 4;
        tempLength = strlen(temp);
    }

    // Replace dots and commas if set for EU
    if (formatType == FORMAT_EU)
        replaceDotsAndCommas(temp);

    // Copy the final result into numberToFormat
	stringCopy(numberToFormat, temp, MAX_STRING_SIZE);
}
*/

// Format currency
void formatCurrency(const double value, char* string, const byte length) {
	if (formatType == FORMAT_US)
		stringCopy(string, currency(value, 2, '.', ',', '$'), length);
	else
		stringCopy(string, currency(value, 2, ',', '.', '$'), length);
}

// Create the scrolling message
void createStockDataMessage(JsonDocument doc) {
	Serial.println("Creating message...");
	const byte MAX_NUMBER_SIZE = 30; // Max length for the numbers
	char tempString[MAX_NUMBER_SIZE]; // Temporary string 1
	char tempString2[MAX_NUMBER_SIZE]; // Temporary string 2
	double tempVal; // Temporary variable 1
	double tempVal2; // Temporary variable 2

	// Price
	Serial.println("Creating price message...");
	tempVal = doc[0]["price"].as<double>();
	tempVal2 = doc[0]["changesPercentage"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	formatCurrency(tempVal2, tempString2, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessagePrice, BUF_SIZE, " BTC: $ %s (%s%%)", tempString, tempString2);

	// Daily Change
	Serial.println("Creating daily change message...");
	tempVal = doc[0]["change"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageDailyChange, BUF_SIZE, "Daily Change: $ %s", tempString);

	// Year High/Low
	Serial.println("Creating year high/low message...");
	tempVal = doc[0]["yearHigh"].as<double>();
	tempVal2 = doc[0]["yearLow"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	formatCurrency(tempVal2, tempString2, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageHighLow, BUF_SIZE, "Year High: $ %s  -  Year Low: $ %s", tempString, tempString2);

	// Open
	Serial.println("Creating open message...");
	tempVal = doc[0]["open"].as<double>();
	formatCurrency(tempVal, tempString, MAX_NUMBER_SIZE); // Format number
	snprintf(stripMessageOpen, BUF_SIZE, "Open: $ %s", tempString);
}

// Getting Bitcoin data
bool getStockDataAPI() {
	/*
	const char host[] = "financialmodelingprep.com";
	char url[100]; // The full URL
	sprintf(url, "https://%s/api/v3/quote/BTCUSD?apikey=%s", host, apiKey); // Create the URL
	if (client.connect(host, 443)) { // Connecting to the server
		http.begin(client, url); // HTTP call
		if (http.GET() == HTTP_CODE_OK) {
			Serial.println("Response body: " + http.getString());
			JsonDocument doc; // Create the JSON object
			DeserializationError error = deserializeJson(doc, http.getString()); // Deserialize the JSON object
			if (error) { // Error while parsing the JSON
				Serial.printf("Error while parsing the JSON: %s\n", error.c_str());
				http.end();
				return false;
			}
			createStockDataMessage(doc); // Create the scrolling message
			http.end(); // Close connection
			return true;
		} else {
			Serial.printf("HTTP call error: %d\n", http.GET());
			http.end();
			return false;
		}
	} else {
		Serial.println("Error while connecting to the host.");
		return false;
	}
	*/

	// const char* host = "financialmodelingprep.com";
    char url[100]; // URL modificato per utilizzare HTTP
    sprintf(url, "http://financialmodelingprep.com/api/v3/quote/BTCUSD?apikey=%s", apiKey);
    http.begin(client, url); // Inizia una chiamata HTTP all'URL specificato
    if (http.GET() == HTTP_CODE_OK) {
        Serial.println("Response body: " + http.getString());
        JsonDocument doc; // Create the JSON object
		DeserializationError error = deserializeJson(doc, http.getString()); // Deserialize the JSON object
		if (error) { // Error while parsing the JSON
			Serial.printf("Error while parsing the JSON: %s\n", error.c_str());
			http.end();
			return false;
		}
		createStockDataMessage(doc); // Create the scrolling message
		http.end(); // Close connection
		return true;
    } else {
        Serial.printf("HTTP call error: %d\n", http.GET());
        http.end();
        return false;
    }
}

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
	return true; // Connection success
}

// Setup delle route
void setupRoutes() {
	server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html"); // Serve web page
	server.onNotFound([](AsyncWebServerRequest *request) { // Page not found
		request->send(404);
	});

	// Connect to WiFi
	server.on("/connect", HTTP_GET, [](AsyncWebServerRequest *request) {
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
		Serial.print("wiFiConnectionStatus: ");
		Serial.println(wiFiConnectionStatus);
		if (connectToWiFi()) { // Connecting to WiFi
			wiFiConnectionStatus = WIFI_OK; // Update connection status
			Serial.print("wiFiConnectionStatus: ");
			Serial.println(wiFiConnectionStatus);
			return true; // Connection success
		} else {
			wiFiConnectionStatus = WIFI_KO; // Update connection status
			Serial.print("wiFiConnectionStatus: ");
			Serial.println(wiFiConnectionStatus);
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

		// Check if connected to WiFi
		if (WiFi.status() == WL_CONNECTED)
			return true; // If connected exit the function

		// Check if credentials are already present
		if (wiFiSSID[0] != '\0' && wiFiPassword[0] != '\0') {
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

// Call the API to get the data
bool callAPI() {
	currentMillis = millis();
	if (currentMillis - timestampStockData > 360000 || timestampStockData == 0) { // Call the API every 6 minutes (To limit usage)
		Serial.println("Calling the API");
		if (!getStockDataAPI()) { // Getting the data
			const char errorMessageServer[] = "Error while calling the API. Retrying...";
			printOnLedMatrix(errorMessageServer, sizeof(errorMessageServer)); // Print the message on the matrix
			return false; // If error, return false
		}
		Serial.println("API called");
		timestampStockData = currentMillis; // Save timestamp
	}
	return true; // If no error, return true
}

// Manage the LED matrix
void manageLedMatrix() {
    if (P.displayAnimate()) { // Is currently scrolling
        Serial.println("End of cycle");
        if (newMessageAvailable) { // Is a new message available?
            stringCopy(curMessage, newMessage, BUF_SIZE); // Store the new message
            newMessageAvailable = false; // Exit the IF statement
        }

        P.displayReset(); // Reset the matrix
		if (checkWifiConnection()) // Check if connected to WiFi
			return; // // If not connected, exit the function
		if (callAPI()) // Call the API
			return; // If error, exit the function

        // Print messagges
        switch (switchText) {
			case PRINT_PRICE:
				Serial.println("Print: PRICE");
				printOnLedMatrix(stripMessagePrice, BUF_SIZE, 30000); // Print the message on the matrix
				switchText = PRINT_CHANGE;
				break;

			case PRINT_CHANGE:
				Serial.println("Print: CHANGE");
				printOnLedMatrix(stripMessageDailyChange, BUF_SIZE); // Print the message on the matrix
				switchText = PRINT_HIGH_LOW;
				break;

			case PRINT_HIGH_LOW:
				Serial.println("Print: HIGHLOW");
				printOnLedMatrix(stripMessageHighLow, BUF_SIZE); // Print the message on the matrix
				switchText = PRINT_OPEN;
				break;

			case PRINT_OPEN:
				Serial.println("Print: OPEN");
				printOnLedMatrix(stripMessageOpen, BUF_SIZE); // Print the message on the matrix
				switchText = PRINT_PRICE;
				break;
        }
    }
}

// Setup the web client
void setupWebClient() {
	// client.setInsecure(); // HTTPS connection
    http.setTimeout(5000); // Set timeout
}

// Setup the LED matrix
void setupLedMatrix() {
	P.begin(); // Start the LED matrix
    sprintf(curMessage, "Initializing...");
    P.displayText(curMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect);
}

// Setup LittleFS
bool setupLittleFS() {
	if (!LittleFS.begin()) { // Check if LittleFS is mounted
		Serial.println("An Error has occurred while mounting LittleFS");
		return false;
	} else {
		return true;
	}
}

// Setup server
void setupServer() {
	setupRoutes(); // Setup routes
	server.begin(); // Start the server
}

// Setup
void setup() {
	Serial.begin(9600); // Start serial
	Serial.println( currency(10000000, 0, '.', ',', '$') );
	Serial.println( currency(10000000, 2, '.', ',', '$') );
	setupLittleFS(); // Setup LittleFS
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