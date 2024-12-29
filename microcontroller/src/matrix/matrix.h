#ifndef MATRIX_H
#define MATRIX_H

#include "../config/config.h"

void printOnLedMatrix(const char* message, const byte stringLength, uint16_t messageStill = scrollPause);
void setupLedMatrix();
void manageLedMatrix();

#endif