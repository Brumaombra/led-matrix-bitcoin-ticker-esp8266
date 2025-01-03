#include "matrix.h"
#include "../config/config.h"
#include "../utils/utils.h"
#include "../api/api.h"
#include "../wifi/wifi.h"
#include "../serial/serial.h"

// Print the message on the matrix
void printOnLedMatrix(const char* message, const byte stringLength, uint16_t messageStill) {
	stringCopy(currentMessage, message, stringLength); // Copy the message
	P.displayText(currentMessage, scrollAlign, scrollDelay, messageStill, scrollEffect, scrollEffect); // Print the message on the matrix
}

// Set the intensity of the matrix
void setMatrixIntensity(uint8_t intensity) {
	P.setIntensity(intensity); // Set LED intensity (0-15, 15 is brightest)
}

// Set the speed of the matrix
void setMatrixSpeed(uint8_t speed) {
    scrollDelay = map(speed, 0, 15, 100, 20); // Map 0-15 to 100-20ms (slower to faster)
}

// Setup the LED matrix
void setupLedMatrix() {
	P.begin(); // Start the LED matrix
	setMatrixIntensity(matrixIntensity); // Set the intensity of the matrix
	printOnLedMatrix("Initializing...", BUF_SIZE); // Print the message on the matrix
}

// Manage the LED matrix
void manageLedMatrix() {
	if (!P.displayAnimate())
		return; // If scrolling, exit the function
	if (!checkWifiConnection()) // Check if connected to WiFi
		return; // If not connected, exit the function
	if (!callAPI()) // Call the API
		return; // If error, exit the function

	// Print messagges
	switch (switchText) {
		case PRINT_PRICE:
			printLogfln("Section: PRICE");
			if (currentPriceVisible) // Check if current price is visible
				printOnLedMatrix(stripMessagePrice, BUF_SIZE, 30000); // Print the message on the matrix
			switchText = PRINT_CHANGE;
			break;

		case PRINT_CHANGE:
			printLogfln("Section: CHANGE");
			if (priceChangeVisible) // Check if price change is visible
				printOnLedMatrix(stripMessageDailyChange, BUF_SIZE); // Print the message on the matrix
			switchText = PRINT_MARKET_CAP;
			break;
		
		case PRINT_MARKET_CAP:
			printLogfln("Section: MARKET CAP");
			if (marketCapVisible) // Check if market cap is visible
				printOnLedMatrix(stripMessageMarketCap, BUF_SIZE); // Print the message on the matrix
			switchText = PRINT_DAILY_HIGH_LOW;
			break;

		case PRINT_DAILY_HIGH_LOW:
			printLogfln("Section: DAILY HIGHLOW");
			if (dailyHighLowVisible) // Check if daily high/low is visible
				printOnLedMatrix(stripMessageDailyHighLow, BUF_SIZE); // Print the message on the matrix
			switchText = PRINT_YEAR_HIGH_LOW;
			break;

		case PRINT_YEAR_HIGH_LOW:
			printLogfln("Section: YEAR HIGHLOW");
			if (yearHighLowVisible) // Check if year high/low is visible
				printOnLedMatrix(stripMessageYearHighLow, BUF_SIZE); // Print the message on the matrix
			switchText = PRINT_OPEN;
			break;

		case PRINT_OPEN:
			printLogfln("Section: OPEN");
			if (openPriceVisible) // Check if open price is visible
				printOnLedMatrix(stripMessageOpen, BUF_SIZE); // Print the message on the matrix
			switchText = PRINT_VOLUME;
			break;
			
		case PRINT_VOLUME:
			printLogfln("Section: VOLUME");
			if (volumeVisible) // Check if volume is visible
				printOnLedMatrix(stripMessageVolume, BUF_SIZE); // Print the message on the matrix
			switchText = PRINT_PRICE;
			break;
	}
}