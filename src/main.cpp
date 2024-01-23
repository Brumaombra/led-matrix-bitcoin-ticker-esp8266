#include <MD_Parola.h>
#include <MD_MAX72xx.h>
// #include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80); // Web server
WiFiClientSecure client; // Client object
HTTPClient http; // HTTP object
const char *accessPointSSID = "Bitcoin-Ticker"; // Access point SSID
String wiFiSSID = ""; // Network SSID
String wiFiPassword = ""; // Network password
bool accessPointEnabled = false; // If access point enabled
bool disableAccessPoint = false; // If I need to disable the access point
enum connectionStatus { WIFI_TRY = 2, WIFI_OK = 1, WIFI_KO = 0 }; // Connection status
connectionStatus wiFiConnectionStatus = WIFI_KO; // Connection status
unsigned long currentMillis; // Current time
unsigned long timestampStockData = 0; // Timestamp stock data
unsigned long timestampWiFiConnection = 0; // Timestamp WiFi connection
String stripMessagePrice; // Price
String dailyChange; // Change
String stripMessageHighLow; // Year High/Low
String stripMessageOpen; // Open
enum formatNum { FORMAT_US = 1, FORMAT_EU = 2 }; // Numeric formatting type
formatNum formatType = FORMAT_US; // Current numeric formatting type
int switchText = 0; // Variable for the switch
String apiKey = ""; // Your API key for financialmodelingprep.com <- TODO - Change with your data

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
bool newMessageAvailable = true; // New available message

// Replace dots and commas
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

// Formatting number
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
	if(formatType == FORMAT_EU)
		numberToFormat = replaceDotsAndCommas(numberToFormat);
    return numberToFormat;
}

/* Formatting percentage
String formatStringPercentage(String percentageToFormat) {
    if (percentageToFormat.indexOf(".") < 0) // Exit if not necessary
        return percentageToFormat + ",00";
    percentageToFormat.replace(".", ","); // Substitute points with commas
    percentageToFormat = percentageToFormat.substring(0, percentageToFormat.indexOf(",") + 3);
    return percentageToFormat;
} */

// Getting Bitcoin data
void getStockDataAPI() {
	String host = "financialmodelingprep.com";
	String url = "https://" + host + "/api/v3/quote/BTCUSD?apikey=" + apiKey;
	if (client.connect(host, 443)) { // Connecting to the server
		http.begin(client, url); // HTTP call
		if (http.GET() == HTTP_CODE_OK) {
			Serial.println("Response body: " + http.getString());
			JsonDocument doc; // Create the JSON object
			DeserializationError error = deserializeJson(doc, http.getString()); // Deserialize the JSON object
			if (error) { // Error while parsing the JSON
				Serial.printf("Error while parsing the JSON: %s\n", error.c_str());
				http.end();
				return;
			}

			// Create the scrolling message
			stripMessagePrice = "BTC: " + formatStringNumber(doc[0]["price"].as<String>()) + " $ (" + formatStringNumber(doc[0]["changesPercentage"].as<String>()) + "%)"; // Price
			dailyChange = "Daily Change: " + formatStringNumber(doc[0]["change"].as<String>()) + " $"; // Daily Change
			stripMessageHighLow = "Year High: " + formatStringNumber(doc[0]["yearHigh"].as<String>()) + " $  -  Year Low: " + formatStringNumber(doc[0]["yearLow"].as<String>()) + " $"; // Year High/Low
			stripMessageOpen = "Open: " + formatStringNumber(doc[0]["open"].as<String>()) + " $"; // Open
			http.end(); // Close connection
		} else {
			Serial.printf("HTTP call error: %d\n", http.GET());
			http.end();
			return;
		}
	} else {
		Serial.println("Error while connecting to the host " + host);
		return;
	}
}

// Connecting to WiFi
bool connectToWiFi() {
	WiFi.begin(wiFiSSID.c_str(), wiFiPassword.c_str()); // Connecting to the WiFi
	int maxTry = 50; // Maximum number of attempts to connect to WiFi
	int count = 0; // Counter
	Serial.print("Connecting to WiFi");
	while (WiFi.status() != WL_CONNECTED) {
		if(count >= maxTry) {
			wiFiSSID = ""; // Reset network SSID
			wiFiPassword = ""; // Reset network password
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
		wiFiSSID = request->getParam("ssid")->value();
		wiFiPassword = request->getParam("password")->value();
		Serial.println("SSID: " + wiFiSSID);
		Serial.println("Password: " + wiFiPassword);
		wiFiConnectionStatus = WIFI_TRY; // Trying to connect
		request->send(200, "application/json", "{\"status\":\"" + String(wiFiConnectionStatus) + "\"}"); // Response
	});

	// Check the WiFi connection status
	server.on("/checkConnection", HTTP_GET, [](AsyncWebServerRequest *request) {
		switch(wiFiConnectionStatus) {
			case WIFI_TRY:
				request->send(200, "application/json", "{\"status\":\"" + String(wiFiConnectionStatus) + "\"}"); // Response
				break;
			case WIFI_OK:
				disableAccessPoint = true; // I need to disable the access point
				request->send(200, "application/json", "{\"status\":\"" + String(wiFiConnectionStatus) + "\"}"); // Response
				break;
			case WIFI_KO:
				request->send(400, "application/json", "{\"status\":\"" + String(wiFiConnectionStatus) + "\"}"); // Response
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
			String json;
			serializeJson(doc, json);
			request->send(200, "application/json", json);
		});
    });
}

// Setting up the access point
bool setupAccessPoint() {
	if(accessPointEnabled) // Check if already enabled
		return true; // If enabled exit the function
	accessPointEnabled = WiFi.softAP(accessPointSSID); // Start the access point
	if(!accessPointEnabled) // Check if enabled
		return false; // If not enabled exit the function
	return true; // Access point enabled
}

// Manage WiFi connection
bool manageWiFiConnection() {
	if(wiFiConnectionStatus == WIFI_TRY) { // Check if already trying to connect
		Serial.println("wiFiConnectionStatus: " + String(wiFiConnectionStatus));
		if(connectToWiFi()) { // Connecting to WiFi
			wiFiConnectionStatus = WIFI_OK; // Update connection status
			Serial.println("wiFiConnectionStatus: " + String(wiFiConnectionStatus));
			return true; // Connection success
		} else {
			wiFiConnectionStatus = WIFI_KO; // Update connection status
			Serial.println("wiFiConnectionStatus: " + String(wiFiConnectionStatus));
			return false; // Connection failed
		}
	}

	// Every 2 seconds
	if (millis() - timestampWiFiConnection > 2000) {
		timestampWiFiConnection = millis(); // Save timestamp
		if(disableAccessPoint && accessPointEnabled) { // Check if I need to disable the access point
			accessPointEnabled = !WiFi.softAPdisconnect(); // Disable access point
			if(!accessPointEnabled) // Check if disabled
				disableAccessPoint = false; // Mark as disabled
		}

		// Check if connected to WiFi
		if (WiFi.status() == WL_CONNECTED)
			return true; // If connected exit the function

		// Check if credentials are already present
		if(wiFiSSID != "" && wiFiPassword != "") {
			if(connectToWiFi()) // Connecting to WiFi
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

// Manage the LED matrix
void manageLedMatrix() {
    if (P.displayAnimate()) { // Is currently scrolling
        Serial.println("End of cycle");
        if (newMessageAvailable) { // Is a new message available?
            strcpy(curMessage, newMessage); // Store the new message
            newMessageAvailable = false; // Exit the IF statement
        }

        P.displayReset();
		
		// Check if connected to WiFi
		if(WiFi.status() != WL_CONNECTED) {
			String errorMessage = "Not connected to Wi-Fi. Use the 'Bitcoin-Ticker' access point to enter the Wi-Fi credentials.";
			Serial.println(errorMessage);
			errorMessage.toCharArray(newMessage, errorMessage.length() + 1); // Copy the error message and convert it to char[]
			P.displayText(newMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect); // Print the error message on the matrix
			newMessageAvailable = true;
			return; // If not connected exit the function
		}

        // Call every 6 minutes (To limit API usage)
        currentMillis = millis();
        if (currentMillis - timestampStockData > 360000 || timestampStockData == 0) {
            timestampStockData = currentMillis; // Save timestamp
			Serial.println("Calling the API");
			getStockDataAPI(); // Getting the data
			Serial.println("API called");
        }

        // Print messagges
        switch (switchText) {
			case 0:
				Serial.println("Print: PRICE");
				stripMessagePrice.toCharArray(newMessage, stripMessagePrice.length() + 1);
				P.displayText(newMessage, scrollAlign, scrollDelay, 30000, scrollEffect, scrollEffect); // Still for 30 seconds
				switchText = 1;
				break;

			case 1:
				Serial.println("Print: CHANGE");
				dailyChange.toCharArray(newMessage, dailyChange.length() + 1);
				P.displayText(newMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect);
				switchText = 2;
				break;

			case 2:
				Serial.println("Print: HIGHLOW");
				stripMessageHighLow.toCharArray(newMessage, stripMessageHighLow.length() + 1);
				P.displayText(newMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect);
				switchText = 3;
				break;

			case 3:
				Serial.println("Print: OPEN");
				stripMessageOpen.toCharArray(newMessage, stripMessageOpen.length() + 1);
				P.displayText(newMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect);
				switchText = 0;
				break;
        }

        newMessageAvailable = true;
    }
}

// Setup the web client
void setupWebClient() {
	client.setInsecure(); // HTTPS connection
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