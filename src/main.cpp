#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <LittleFS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80); // Web server
AsyncWebSocket ws("/ws"); // Web socket
WiFiClientSecure client; // Client object
HTTPClient http; // HTTP object
const char *accessPointSSID = "Bitcoin-Ticker"; // Access point SSID
String wiFiSSID = ""; // Network SSID
String wiFiPassword = ""; // Network password
bool accessPointEnabled = false; // If access point enabled
unsigned long currentMillis; // Current time
unsigned long timestampStockData = 0; // Timestamp stock data
unsigned long timestampWiFiConnection = 0; // Timestamp WiFi connection
String stripMessagePrice; // Price
String dailyChange; // Change
String stripMessageHighLow; // Year High/Low
String stripMessageOpen; // Open
int switchText = 0; // Variable for the switch
String apiKey = "8b5e75db3ac93df1144f0743f2b5b786"; // Your API key for financialmodelingprep.com <- TODO - Change with your data

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

// Formatting number
String formatStringNumber(String numberToFormat) {
    if (numberToFormat.indexOf(".") < 0) // Adding decimals if needed
        numberToFormat += ",00";
    numberToFormat.replace(".", ","); // Substitute points with commas (I'm italian XD)

    // Formatting decimals
    if (numberToFormat.length() - numberToFormat.indexOf(",") < 3)
        numberToFormat += "0";
    else
        numberToFormat = numberToFormat.substring(0, numberToFormat.indexOf(",") + 3);

    // Adding thousands separator
    unsigned int currLength = 6;
    while (numberToFormat.length() > currLength) {
        numberToFormat = numberToFormat.substring(0, numberToFormat.length() - currLength) + "." + numberToFormat.substring(numberToFormat.length() - currLength);
        currLength += 4;
    }
    return numberToFormat;
}

// Formatting percentage
String formatStringPercentage(String percentageToFormat) {
    if (percentageToFormat.indexOf(".") < 0) // Exit if not necessary
        return percentageToFormat + ",00";
    percentageToFormat.replace(".", ","); // Substitute points with commas
    percentageToFormat = percentageToFormat.substring(0, percentageToFormat.indexOf(",") + 3);
    return percentageToFormat;
}

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
			stripMessagePrice = "BTC: " + formatStringNumber(doc[0]["price"].as<String>()) + " $ (" + formatStringPercentage(doc[0]["changesPercentage"].as<String>()) + "%)_"; // Price
			dailyChange = "Daily Change: " + formatStringNumber(doc[0]["change"].as<String>()) + " $_"; // Daily Change
			stripMessageHighLow = "Year High: " + formatStringNumber(doc[0]["yearHigh"].as<String>()) + " $  -  Year Low: " + formatStringNumber(doc[0]["yearLow"].as<String>()) + " $_"; // Year High/Low
			stripMessageOpen = "Open: " + formatStringNumber(doc[0]["open"].as<String>()) + " $_"; // Open
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
bool connectToWiFi(const String &wiFiSSIDTemp = "", const String &wiFiPasswordTemp = "") {
	String wiFiSSIDTest = wiFiSSIDTemp != "" ? wiFiSSIDTemp : wiFiSSID; // Test network SSID
	String wiFiPasswordTest = wiFiPasswordTemp != "" ? wiFiPasswordTemp : wiFiPassword; // Test network password
	WiFi.begin(wiFiSSIDTest.c_str(), wiFiPasswordTest.c_str()); // Connecting to the WiFi
	int maxTry = 50; // Maximum number of attempts to connect to WiFi
	int count = 0; // Counter
	Serial.print("Connecting to WiFi");
	ws.textAll("WIFI_TRY"); // Send message
	while (WiFi.status() != WL_CONNECTED) {
		if(count >= maxTry)
			return false; // Connection failed
		count++;
		Serial.print(".");
		delay(250);
	}
	wiFiSSID = wiFiSSIDTest; // Update network SSID
	wiFiPassword = wiFiPasswordTest; // Update network password
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
		String ssid = request->getParam("ssid")->value();
		String password = request->getParam("password")->value();
		Serial.println("SSID: " + ssid);
		Serial.println("Password: " + password);
		WiFi.begin(ssid.c_str(), password.c_str());
		ws.textAll("WIFI_TRY"); // Send message
		request->send(200, "application/json", "{\"status\":\"OK\"}"); // Success
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
	if (millis() - timestampWiFiConnection > 2000) { // Every 2 seconds
		timestampWiFiConnection = millis(); // Save timestamp
		if (WiFi.status() == WL_CONNECTED) { // Check if connected to WiFi
			ws.textAll("WIFI_CON"); // Send message
			return true; // If connected exit the function
		}

		// Check if credentials are already present
		if(wiFiSSID != "" && wiFiPassword != "") {
			if(connectToWiFi()) { // Connecting to WiFi
				ws.textAll("WIFI_CON"); // Send message
				return true; // Connection success
			} else {
				setupAccessPoint(); // Setup access point
			}
		} else {
			setupAccessPoint(); // Setup access point
		}
		ws.textAll("WIFI_DIS"); // Send message
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
				stripMessagePrice.toCharArray(newMessage, stripMessagePrice.length());
				P.displayText(newMessage, scrollAlign, scrollDelay, 30000, scrollEffect, scrollEffect); // Still for 30 seconds
				switchText = 1;
				break;

			case 1:
				Serial.println("Print: CHANGE");
				dailyChange.toCharArray(newMessage, dailyChange.length());
				P.displayText(newMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect);
				switchText = 2;
				break;

			case 2:
				Serial.println("Print: HIGHLOW");
				stripMessageHighLow.toCharArray(newMessage, stripMessageHighLow.length());
				P.displayText(newMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect);
				switchText = 3;
				break;

			case 3:
				Serial.println("Print: OPEN");
				stripMessageOpen.toCharArray(newMessage, stripMessageOpen.length());
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

// Manage the different types of requests
void manageResponse(String message, AsyncWebSocketClient *client) {
	JsonDocument doc; // JSON object
	if (deserializeJson(doc, message)) { // Check if there is an error
		Serial.print("Error during the JSON parsing");
		return;
	}

	// Response switch
	String type = doc["type"]; // Get the response type
};

// Web socket event
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	switch(type) { // Message type
		case WS_EVT_CONNECT: // Client connected
			Serial.println("Websocket client connection received");
			break;
		case WS_EVT_DISCONNECT: // Client disconnected
			Serial.println("Client disconnected");
			break;
		case WS_EVT_ERROR: // Error
			Serial.println("Error");
			break;
		case WS_EVT_PONG:
			Serial.println("Pong received");
			break;
		case WS_EVT_DATA: // Data received
			String message = "";
			for(size_t i = 0; i < len; i++) // Create the message
				message += (char) data[i];
			manageResponse(message, client);
			Serial.print("Data received: " + message);
			break;
	}
}

// Setup server
void setupServer() {
	setupRoutes(); // Setup routes
	ws.onEvent(onWsEvent); // Web socket event
  	server.addHandler(&ws); // Web socket handler
	server.begin(); // Start the server
}

// Setup
void setup() {
	Serial.begin(9600); // Start serial
	setupLittleFS(); // Setup LittleFS
	setupServer(); // Setup server
	manageWiFiConnection(); // Manage WiFi connection (Pass by if connected to WiFi, otherwise handle the connection process)
	setupWebClient(); // Setup web client
	setupLedMatrix(); // Setup LED matrix
}

// Loop
void loop() {
	manageWiFiConnection(); // Manage WiFi connection (Pass by if connected to WiFi, otherwise handle the connection process)
	manageLedMatrix(); // Manage the LED matrix
}