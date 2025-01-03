#ifndef CONFIG_H
#define CONFIG_H

#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <ESPAsyncWebServer.h>

// Hardware configuration
constexpr MD_MAX72XX::moduleType_t HARDWARE_TYPE = MD_MAX72XX::FC16_HW;
constexpr uint16_t BUF_SIZE = 250; // Buffer length
constexpr uint8_t MAX_DEVICES = 16; // Number of modules
constexpr uint8_t CLK_PIN = D5; // SCK
constexpr uint8_t DATA_PIN = D7; // MOSI
constexpr uint8_t CS_PIN = D8; // SS

// Numeric formatting type
enum formatNum { FORMAT_US = 1, FORMAT_EU = 2 };
extern formatNum formatType;

// Global message buffers shared by serial and scrolling functions
extern char currentMessage[BUF_SIZE]; // Current message
extern char stripMessagePrice[BUF_SIZE]; // Price
extern char stripMessageDailyChange[BUF_SIZE]; // Change
extern char stripMessageMarketCap[BUF_SIZE]; // Market Cap
extern char stripMessageDailyHighLow[BUF_SIZE]; // Daily High/Low
extern char stripMessageYearHighLow[BUF_SIZE]; // Year High/Low
extern char stripMessageOpen[BUF_SIZE]; // Open
extern char stripMessageVolume[BUF_SIZE]; // Volume

// WiFi and client configuration
extern AsyncWebServer server;
extern WiFiClient client;
extern const char accessPointSSID[];
extern char wiFiSSID[35];
extern char wiFiPassword[70];
extern char apiKey[35];
extern bool accessPointEnabled;
extern bool disableAccessPoint;
enum connectionStatus { WIFI_TRY = 2, WIFI_OK = 1, WIFI_KO = 0 };
extern connectionStatus wiFiConnectionStatus;
extern unsigned long currentMillis;
extern unsigned long timestampStockData;
extern unsigned long timestampWiFiConnection;

// Scrolling text configuration
enum printType { PRINT_PRICE = 0, PRINT_CHANGE = 1, PRINT_MARKET_CAP = 2, PRINT_DAILY_HIGH_LOW = 3, PRINT_YEAR_HIGH_LOW = 4, PRINT_OPEN = 5, PRINT_VOLUME = 6 };
extern printType switchText;
extern MD_Parola P;
extern uint8_t scrollDelay;
extern textEffect_t scrollEffect;
extern textPosition_t scrollAlign;
extern uint16_t scrollPause;

// Visibility booleans
extern bool currentPriceVisible;
extern bool priceChangeVisible;
extern bool marketCapVisible;
extern bool dailyHighLowVisible;
extern bool yearHighLowVisible;
extern bool openPriceVisible;
extern bool volumeVisible;
extern uint8_t matrixIntensity;
extern uint8_t scrollSpeed;

#endif