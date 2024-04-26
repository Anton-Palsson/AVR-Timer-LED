#ifndef SETUP_H
#define SETUP_H

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
const int ledPin = 3;

// Functions
void setup();
void processSerialCommand();
void processTimerInterrupt();

#endif