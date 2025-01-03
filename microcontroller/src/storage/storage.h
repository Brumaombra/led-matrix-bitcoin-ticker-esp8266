#ifndef STORAGE_H
#define STORAGE_H

#include <ArduinoJson.h>

bool setupLittleFS();
void setupEEPROM();
bool readEEPROM(JsonDocument& doc);
void testReadEEPROM();
bool writeEEPROM();

#endif