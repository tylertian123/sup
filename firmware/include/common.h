#pragma once

#include <Arduino.h>

#define DEBUG_OUT(x) Serial.println(x)
#define DEBUG_OUT_F(x, ...) Serial.printf(x, __VA_ARGS__)
#define DEBUG_OUT_FP(x, ...) Serial.printf_P(x, __VA_ARGS__)
#define STATUS_LED D0
