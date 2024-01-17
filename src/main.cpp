#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Setup access point and WiFi
const char *accessPointSSID = "Bitcoin-Ticker"; // Access point SSID
String wiFiSSID = ""; // Network SSID
String wiFiPassword = ""; // Network password
bool accessPointEnabled = false; // If access point enabled
ESP8266WebServer server(80);

WiFiClientSecure client; // Client object
HTTPClient http; // HTTP object
unsigned long currentMillis; // Current time
unsigned long timestampStockData = 0; // Timestamp stock data
String stripMessagePrice; // Price
String dailyChange; // Change
String stripMessageHighLow; // Year High/Low
String stripMessageOpen; // Open
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

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); //  (HARDWARE SPI)
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
    if (numberToFormat.indexOf(".") < 0) { // Adding decimals if needed
        numberToFormat += ",00";
    }

    numberToFormat.replace(".", ","); // Substitute points with commas (I'm italian XD)

    // Formatting decimals
    if (numberToFormat.length() - numberToFormat.indexOf(",") < 3) {
        numberToFormat += "0";
    } else {
        numberToFormat = numberToFormat.substring(0, numberToFormat.indexOf(",") + 3);
    }

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
    if (percentageToFormat.indexOf(".") < 0) { // Exit if not necessary
        return percentageToFormat + ",00";
    }

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

			// Creating the JSON object
			JsonDocument doc; // (ESP.getMaxFreeBlockSize() - 512);
			DeserializationError error = deserializeJson(doc, http.getString());

			if (error) {
				Serial.printf("Error while parsing the JSON: %s\n", error.c_str());
				http.end();
				return;
			}

			// Create the scrolling message
			stripMessagePrice = "BTC: " + formatStringNumber(doc[0]["price"].as < String > ()) + " $ (" + formatStringPercentage(doc[0]["changesPercentage"].as < String > ()) + "%)_"; // Price
			dailyChange = "Daily Change: " + formatStringNumber(doc[0]["change"].as < String > ()) + " $_"; // Daily Change
			stripMessageHighLow = "Year High: " + formatStringNumber(doc[0]["yearHigh"].as < String > ()) + " $  -  Year Low: " + formatStringNumber(doc[0]["yearLow"].as < String > ()) + " $_"; // Year High/Low
			stripMessageOpen = "Open: " + formatStringNumber(doc[0]["open"].as < String > ()) + " $_"; // Open
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

// WiFi credential page
String getHTML() {
	String html = "<html><body>";
	html += "<form action='/setup' method='post'>";
	html += "SSID: <input type='text' name='ssid'><br>";
	html += "Password: <input type='password' name='password'><br>";
	html += "<input type='submit' value='Connetti'>";
	html += "</form></body></html>";
	return html;
}

// Connecting to WiFi
bool connectToWiFi() {
	WiFi.begin(wiFiSSID.c_str(), wiFiPassword.c_str());
	int maxTry = 10; // Maximum number of attempts to connect to WiFi
	int count = 0; // Counter
	while (WiFi.status() != WL_CONNECTED) {
		if(count >= maxTry)
			return false; // Connection failed
		count++;
		delay(1000);
	}
	return true; // Connection success
}

// Setting up the access point
bool setupAccessPoint() {
	if(accessPointEnabled) // Check if already enabled
		return true; // If enabled exit the function
	accessPointEnabled = WiFi.softAP(accessPointSSID); // Start the access point
	if(!accessPointEnabled) // Check if enabled
		return false; // If not enabled exit the function
	server.on("/", HTTP_GET, []() { // Route root
		server.send(200, "text/html", getHTML());
	});
	server.on("/setup", HTTP_POST, []() { // Route for setting up the WiFi credentials
		wiFiSSID = server.arg("ssid");
		wiFiPassword = server.arg("password");
		bool connected = connectToWiFi(); // Connecting to WiFi
		if(connected) { // Check if connected to WiFi
			server.send(200, "text/html", "OK"); // Success
			accessPointEnabled = !WiFi.softAPdisconnect(true); // Access point disabled
		} else {
			server.send(500, "text/html", "KO"); // Error
		}
	});
	server.begin(); // Start the server
	return true; // Access point enabled
}

// Manage WiFi connection
bool manageWiFiConnection() {
	if (WiFi.status() == WL_CONNECTED) // Check if connected to WiFi
		return true; // If connected exit the function

	// Check if credentials are present
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
			String errorMessage = "Network error, not connected to WiFi";
			Serial.println(errorMessage);
			errorMessage.toCharArray(newMessage, errorMessage.length() + 1); // Copy the error message and convert it to char[]
			P.displayText(newMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect); // Print the error message on the matrix
			newMessageAvailable = true;
			return; // If not connected exit the function
		}

        // Call every 6 minutes (For limiting API usage)
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
	P.begin();
    sprintf(curMessage, "Initializing...");
    P.displayText(curMessage, scrollAlign, scrollDelay, scrollPause, scrollEffect, scrollEffect);
}

// Setup
void setup() {
	Serial.begin(57600); // Start serial communication
	manageWiFiConnection(); // Manage WiFi connection (Pass by if connected to WiFi, otherwise handle the connection process)
	setupWebClient(); // Setup web client
	setupLedMatrix(); // Setup LED matrix
}

// Loop
void loop() {
	server.handleClient(); // Request handling
	manageWiFiConnection(); // Manage WiFi connection (Pass by if connected to WiFi, otherwise handle the connection process)
	manageLedMatrix(); // Manage the LED matrix
}