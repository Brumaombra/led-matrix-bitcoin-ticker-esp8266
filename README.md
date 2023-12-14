# Bitcoin Price Ticker for MAX72XX LED Matrix

## Overview
This project displays live Bitcoin prices and various metrics on a MAX72XX LED matrix using an ESP8266 microcontroller. It retrieves data from a financial API and shows information such as the current price, daily change, yearly high/low, and the opening price.

## Requirements
- ESP8266 WiFi Module
- MAX72XX LED Matrix
- Arduino IDE
- Libraries:
  - MD_Parola
  - MD_MAX72xx
  - ArduinoJson
  - ESP8266WiFi
  - ESP8266HTTPClient
  - WiFiClientSecure

## Installation
1. Install Arduino IDE from [the official website](https://www.arduino.cc/en/software).
2. Open Arduino IDE, go to `Sketch > Include Library > Manage Libraries`.
3. Install the following libraries: `MD_Parola`, `MD_MAX72xx`, `ArduinoJson`, `ESP8266WiFi`, `ESP8266HTTPClient`, `WiFiClientSecure`.
4. Connect your ESP8266 module to your computer.
5. Copy the provided code into a new Arduino sketch.

## Configuration
Before running the sketch, make sure to configure the following parameters:
- `apiKey`: Your API key for financialmodelingprep.com
- `MAX_DEVICES`: Number of modules in your LED matrix
- `CLK_PIN`, `DATA_PIN`, `CS_PIN`: Pin configuration for your ESP8266
- `ssid`: Your WiFi network name
- `password`: Your WiFi network password

## Usage
Upload the sketch to your ESP8266 module. Once uploaded and running, the device will connect to your WiFi network, fetch Bitcoin price data from the API, and display it on the LED matrix.

## Customization
You can customize the display settings such as scroll speed, delay, and text effects by modifying the corresponding variables in the code.

## Troubleshooting
Ensure that your ESP8266 is correctly connected and that the correct port is selected in the Arduino IDE. If you encounter issues with API connectivity, verify your API key and internet connection.

## Author
Mauro Brambilla - April 2023

## License
This project is licensed under the MIT License - see the LICENSE file for details.