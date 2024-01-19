
# ESP8266 LED Matrix Bitcoin Ticker

## Overview
This project turns an ESP8266 microcontroller and a MAX7219 LED matrix into a dynamic Bitcoin ticker. It displays the latest Bitcoin price information, providing a unique and informative crypto display.

## Features
- **Real-Time Bitcoin Price Display:** Shows up-to-date Bitcoin prices.
- **WiFi Connectivity:** Connects to the internet to fetch real-time data.
- **Web Server for Configuration:** Includes a web server for easy WiFi configuration.
- **Scrolling Text Display:** Displays various financial data in a scrolling format.

## Hardware Requirements
- ESP8266 Microcontroller
- MAX7219 LED Matrix
- Power supply for the ESP8266

## Hardware Configuration
- **LED Matrix Control:** Uses `MD_Parola` and `MD_MAX72xx` libraries.
- **Pin Configuration:** Customize CLK, DATA, and CS pin connections.
- **Modular Setup:** Supports a variable number of LED matrix modules.

## Software Dependencies
- PlatformIO
- Libraries: MD_Parola, MD_MAX72xx, SPI, ESP8266WiFi, ESP8266WebServer, ESP8266HTTPClient, ArduinoJson, WiFiClientSecure, LittleFS.

## Installation
1. Connect the MAX7219 LED matrix to the ESP8266 following the specified pin configuration.
2. Compile and upload the `main.cpp` file to the ESP8266 using PlatformIO.
3. Place the contents of the `data` folder in the root directory of the ESP8266's file system.

## Configuration and Usage
1. Initially, the ESP8266 will create an access point named "Bitcoin-Ticker."
2. Connect to this access point and access the ESP8266's IP in a browser to configure WiFi settings.
3. Enter WiFi network credentials; the device will restart and connect to your network.
4. The LED matrix will then start showing the current Bitcoin price and other financial data.

## API Configuration
- **Financial Data Source:** The ticker fetches data from `financialmodelingprep.com`.
- **API Key Required:** Users must provide their API key for data access.

## Customization Options
- **Number of LED Modules:** Adjustable based on your setup.
- **Pinout Configuration:** Modify CLK, DATA, and CS pins as needed.
- **Scroll Speed:** Adjust the scroll delay for the text display.

For detailed setup and customization instructions, refer to comments within the `main.cpp` file.

## Author
Mauro Brambilla (brumaombra)

## License
This project is open-source and available under MIT License.
