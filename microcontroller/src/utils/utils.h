#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

void stringCopy(char* destination, const char* text, size_t length);
char* addThousandsSeparators(double value, int decimals, char decimalSeparator, char thousandSeparator, char symbol = ' ');
void formatCurrency(double value, char* output, const byte length);

#endif