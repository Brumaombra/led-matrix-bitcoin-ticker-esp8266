#include "config.h"

// Numeric formatting type
formatNum formatType = FORMAT_US;

// Global message buffers shared by serial and scrolling functions
char currentMessage[BUF_SIZE]; // Current message
char stripMessagePrice[BUF_SIZE]; // Price
char stripMessageDailyChange[BUF_SIZE]; // Change
char stripMessageMarketCap[BUF_SIZE]; // Market Cap
char stripMessageDailyHighLow[BUF_SIZE]; // Daily High/Low
char stripMessageYearHighLow[BUF_SIZE]; // Year High/Low
char stripMessageOpen[BUF_SIZE]; // Open
char stripMessageVolume[BUF_SIZE]; // Volume

// WiFi and client configuration
AsyncWebServer server(80);
WiFiClient client;
const char accessPointSSID[] = "Bitcoin-Ticker";
char wiFiSSID[35] = "";
char wiFiPassword[70] = "";
char apiKey[35] = "";
bool accessPointEnabled = false;
bool disableAccessPoint = false;
connectionStatus wiFiConnectionStatus = WIFI_KO;
unsigned long currentMillis;
unsigned long timestampStockData = 0;
unsigned long timestampWiFiConnection = 0;

// Scrolling text configuration
printType switchText = PRINT_PRICE;
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
uint8_t scrollDelay = 30;
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 0;

// Visibility booleans
bool currentPriceVisible = true;
bool priceChangeVisible = true;
bool marketCapVisible = true;
bool dailyHighLowVisible = true;
bool yearHighLowVisible = true;
bool openPriceVisible = true;
bool volumeVisible = true;
uint8_t matrixIntensity = 1;
uint8_t scrollSpeed = 1;